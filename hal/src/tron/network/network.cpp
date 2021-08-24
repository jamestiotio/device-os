/*
 * Copyright (c) 2018 Particle Industries, Inc.  All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#define NO_STATIC_ASSERT
#include "ifapi.h"
#include "wiznet/wiznetif.h"
#include <mutex>
#include <memory>
#include "random.h"
#include "check.h"
#include <malloc.h>
#include "network/ncp_client/realtek/rtl_ncp_client.h"
#include "network/ncp/wifi/wifi_network_manager.h"
#include "network/ncp/wifi/ncp.h"
#include "debug.h"
#include "realtek/rtlncpnetif.h"
#include "lwip_util.h"
#include "core_hal.h"

using namespace particle;
using namespace particle::net;

namespace particle {

namespace {

/* en2 - Ethernet FeatherWing */
BaseNetif* en2 = nullptr;
/* wl3 - Realtek NCP Station */
BaseNetif* wl3 = nullptr;
/* wl4 - Realtek NCP Access Point */
BaseNetif* wl4 = nullptr;

class WifiNetworkManagerInit {
public:
    WifiNetworkManagerInit() {
        const int ret = init();
        SPARK_ASSERT(ret == 0);
    }

    WifiNetworkManager* instance() const {
        return mgr_.get();
    }

private:
    std::unique_ptr<WifiNcpClient> ncpClient_;
    std::unique_ptr<WifiNetworkManager> mgr_;

    int init() {
        // Initialize NCP client
        std::unique_ptr<WifiNcpClient> ncpClient(new(std::nothrow) RealtekNcpClient);
        CHECK_TRUE(ncpClient, SYSTEM_ERROR_NO_MEMORY);
        auto conf = NcpClientConfig()
                .eventHandler(RealtekNcpNetif::ncpEventHandlerCb, wl3)
                .dataHandler(RealtekNcpNetif::ncpDataHandlerCb, wl3);
        CHECK(ncpClient->init(std::move(conf)));
        // Initialize network manager
        mgr_.reset(new(std::nothrow) WifiNetworkManager(ncpClient.get()));
        CHECK_TRUE(mgr_, SYSTEM_ERROR_NO_MEMORY);
        ncpClient_ = std::move(ncpClient);
        return 0;
    }
};

bool netifCanForwardIpv4(netif* iface) {
    if (iface && netif_is_up(iface) && netif_is_link_up(iface)) {
        auto addr = netif_ip_addr4(iface);
        auto mask = netif_ip_netmask4(iface);
        auto gw = netif_ip_gw4(iface);
        if (!ip_addr_isany(addr) && !ip_addr_isany(mask) && !ip_addr_isany(gw)) {
            return true;
        }
    }

    return false;
}

} // unnamed

WifiNetworkManager* wifiNetworkManager() {
    static WifiNetworkManagerInit mgr;
    return mgr.instance();
}

} // particle

int if_init_platform(void*) {
    /* lo0 (created by LwIP) */

    /* th1 - OpenThread (Deprecated) */
    reserve_netif_index();

    /* en2 - Ethernet FeatherWing (optional) */
    uint8_t mac[6] = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
    {
        // const uint32_t lsb = __builtin_bswap32(NRF_FICR->DEVICEADDR[0]);
        // const uint32_t msb = NRF_FICR->DEVICEADDR[1] & 0xffff;
        // memcpy(mac + 2, &lsb, sizeof(lsb));
        // mac[0] = msb >> 8;
        // mac[1] = msb;
        /* Drop 'multicast' bit */
        mac[0] &= 0b11111110;
        /* Set 'locally administered' bit */
        mac[0] |= 0b10;
    }

    if (HAL_Feature_Get(FEATURE_ETHERNET_DETECTION)) {
#if PLATFORM_ID == PLATFORM_ARGON || PLATFORM_ID == PLATFORM_TRON
        en2 = new WizNetif(HAL_SPI_INTERFACE1, D5, D3, D4, mac);
#else // A SoM
        en2 = new WizNetif(HAL_SPI_INTERFACE1, D8, A7, D22, mac);
#endif
    }

    uint8_t dummy;
    if (!en2 || if_get_index(en2->interface(), &dummy)) {
        /* No en2 present */
        delete en2;
        en2 = nullptr;
        reserve_netif_index();
    }

    /* wl3 - Realtek NCP Station */
    wl3 = new RealtekNcpNetif();
    if (wl3) {
        ((RealtekNcpNetif*)wl3)->setWifiManager(wifiNetworkManager());
        ((RealtekNcpNetif*)wl3)->init();
    }

    /* TODO: wl4 - ESP32 NCP Access Point */
    (void)wl4;

    auto m = mallinfo();
    const size_t total = m.uordblks + m.fordblks;
    LOG(TRACE, "Heap: %lu/%lu Kbytes used", m.uordblks / 1000, total / 1000);

    return 0;
}


extern "C" {

struct netif* lwip_hook_ip4_route_src(const ip4_addr_t* src, const ip4_addr_t* dst) {
    if (src == nullptr) {
        if (en2 && netifCanForwardIpv4(en2->interface())) {
            return en2->interface();
        } else if (wl3 && netifCanForwardIpv4(wl3->interface())) {
            return wl3->interface();
        }
    }

    return nullptr;
}

unsigned char* rltk_wlan_get_ip(int idx) {
    uint8_t* p = (uint8_t*)&wl3->interface()->ip_addr;
    (void)p;
    LOG(INFO, "rltk_wlan_get_ip: idx: %d, ip: %d, %d, %d, %d", idx, p[0], p[1], p[2], p[3]);

    return (uint8_t *) &(wl3->interface()->ip_addr);
}

unsigned char* rltk_wlan_get_gw(int idx) {
    LOG(INFO, "rltk_wlan_get_gw: 0x%x", wl3->interface()->gw.u_addr.ip4.addr);
    return (uint8_t *) &(wl3->interface()->gw);
}

unsigned char* rltk_wlan_get_gwmask(int idx) {
    LOG(INFO, "rltk_wlan_get_gwmask: 0x%x", wl3->interface()->netmask.u_addr.ip4.addr);
    return (uint8_t *) &(wl3->interface()->netmask);
}

void rltk_wlan_set_netif_info(int idx_wlan, void * dev, unsigned char * dev_addr) {
    LOG(INFO, "rltk_wlan_set_netif_info %d", idx_wlan);
}

void netif_rx(int idx, unsigned int len) {
    // LOG(INFO, "netif_rx %d %u", idx, len);
    RealtekNcpNetif::ncpDataHandlerCb(0, nullptr, len, wl3);
}

int netif_is_valid_IP(int idx, unsigned char *ip_dest) {
	struct netif * pnetif = wl3->interface();

	ip_addr_t addr = {};

	u32_t *ip_dest_addr  = (u32_t*)ip_dest;

	LOG(INFO, "netif_is_valid_IP, IP: %d.%d.%d.%d ",ip_dest[0],ip_dest[1],ip_dest[2],ip_dest[3]);

    // Prevent the warning: “the address of XXX will never be NULL”
    ip_addr_t* p = &addr;
	ip_addr_set_ip4_u32(p, *ip_dest_addr);
	if((ip_addr_get_ip4_u32(netif_ip_addr4(pnetif))) == 0)
		return 1;

	if(ip_addr_ismulticast(&addr) || ip_addr_isbroadcast(&addr,pnetif)){
		return 1;
	}

	//if(ip_addr_netcmp(&(pnetif->ip_addr), &addr, &(pnetif->netmask))) //addr&netmask
	//	return 1;

	if(ip_addr_cmp(&(pnetif->ip_addr),&addr))
		return 1;

	LOG(INFO, "invalid IP: %d.%d.%d.%d ",ip_dest[0],ip_dest[1],ip_dest[2],ip_dest[3]);

	return 0;
}

}
