/* Author: Garrett Scholtes
 * Date:   2015-11-30
 * 
 * Lab6 - CS4029
 * 
 * This module serves as pseudo network device driver
 * Does not handle ARP requests
 * 
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/etherdevice.h>

struct os_priv {
    struct net_device_stats stats;
    struct sk_buff *skb;
    struct os_packet *pkt;
    struct net_device *dev;
};

struct os_packet {
    struct net_device *dev;
    int datalen;
    u8 data[ETH_DATA_LEN];
};

MODULE_LICENSE("GPL");

struct net_device *os0;
struct net_device *os1;

struct os_priv *priv0;
struct os_priv *priv1;

//////// Device operations ////////

int os_open(struct net_device *dev) { return 0; }

int os_stop(struct net_device *dev) { return 0; }

int os_start_xmit(struct sk_buff *skb, struct net_device *dev) { return 0; }

struct net_device_stats *os_stats(struct net_device *dev) {
  return &(((struct os_priv*)netdev_priv(dev))->stats);
}

//////// Header operations ////////

int os_header(struct sk_buff *skb, struct net_device *dev,
              unsigned short type, const void *daddr, const void *saddr,
              unsigned int len) {
    return 0;
}


// Point to the device operations
static const struct net_device_ops os_device_ops = {
    .ndo_open = os_open,
    .ndo_stop = os_stop,
    .ndo_start_xmit = os_start_xmit,
    .ndo_get_stats = os_stats
};

// Point to the header operations
static const struct header_ops os_header_ops = {
    .create = os_header
};

int init_module(void) {
    int i;

    printk("netdriver module installing\n");

    // Allocate space for network device interfaces
    os0 = alloc_etherdev(sizeof(struct os_priv));
    os1 = alloc_etherdev(sizeof(struct os_priv));

    // Set MAC address, broadcast, and header length for devices
    for(i = 0; i < 6; i++) {
        os0->dev_addr[i] = (unsigned char)i;   // 00:01:02:03:04:05
        os1->dev_addr[i] = (unsigned char)i;   // See below
        os0->broadcast[i] = (unsigned char)15; // FF:FF:FF:FF:FF:FF
        os1->broadcast[i] = (unsigned char)15; // FF:FF:FF:FF:FF:FF
    }
    os0->hard_header_len = 14;
    os1->hard_header_len = 14;
    os1->dev_addr[5]++; // 00:01:02:03:04:06

    // Fill in name field of interfaces
    memcpy(os0->name, "os0\0", 4);
    memcpy(os1->name, "os1\0", 4);

    // Give network device interfaces some operations
    os0->netdev_ops = &os_device_ops;
    os1->netdev_ops = &os_device_ops;

    // And now give them some header ops
    os0->header_ops = &os_header_ops;
    os1->header_ops = &os_header_ops;

    // Disable ARP
    /* TODO: if time permits, implement ARP handles */
    os0->flags |= IFF_NOARP;
    os1->flags |= IFF_NOARP;

    // Private data
    priv0 = netdev_priv(os0);
    priv1 = netdev_priv(os1);
    memset(priv0, 0, sizeof(struct os_priv));
    memset(priv1, 0, sizeof(struct os_priv));
    priv0->pkt = kmalloc(sizeof(struct os_packet), GFP_KERNEL);
    priv1->pkt = kmalloc(sizeof(struct os_packet), GFP_KERNEL);
    priv0->pkt->dev = os0;
    priv1->pkt->dev = os1;

    // Finally register the interfaces
    register_netdev(os0);
    register_netdev(os1);

    return 0;
}

void cleanup_module(void) {
    printk("netdriver module uninstalling\n");

    if(os0) { 
        kfree(priv0->pkt);
        unregister_netdev(os0);
    }

    if(os1) {
        kfree(priv1->pkt);
        unregister_netdev(os1);
    }
    return;
}