#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/slab.h>
#include <linux/inet.h>
#include <linux/time.h>

#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/icmp.h>

static struct nf_hook_ops nf_forw;

struct loc_time_tag
{
  int  ttl;
  int latitude;
  int longitude;
  unsigned int timestamp;
};

#define ICMP     0x01
#define UDP      0x11
#define TCP      0x06
#define SIZE sizeof(struct loc_time_tag)


struct timeval forwardingtime(void)
{
  struct timeval currtime;

  do_gettimeofday(&currtime);
  
  return currtime;
}


int forwardingCoord(float a)
{
  int degree = (int)a;
  if(degree < 0)
    a = ((float)degree - a) * 60;
  else
    a = (a - (float)degree) * 60;
  return (int)(((degree*60)+a) * 30000);
}


unsigned int forwarding_hook_func(unsigned int hooknum, struct sk_buff *skb, const struct net_device *in, const struct net_device *out, int(*okfn)(struct sk_buff *))
{
  struct timeval currtime;
  struct loc_time_tag *ptr;
  struct iphdr *ip_h;
  struct udphdr *udp_h;
  unsigned char *data_ptr;
  unsigned int data_len;
  int length;
  int offset;

  currtime = forwardingtime();
  ptr = kmalloc(SIZE, GFP_ATOMIC);
  ptr->latitude  = forwardingCoord(35.769864); 
  ptr->longitude = forwardingCoord(-78.675421); 
  ptr->timestamp = (u32)currtime.tv_sec;
  ip_h = (struct iphdr *)skb_network_header(skb);
  if(ip_h->tos & 0x7C)
  {
    printk(KERN_ALERT "\n   forwarding_hook_func @ work **/ \n"); 
    switch(ip_h->protocol)
    {
	case TCP:
		printk(KERN_ALERT " TCP PACKET FORWARDED \n");		  
		break;
		
	case UDP:		
		printk(KERN_ALERT " UDP PACKET FORWARDED \n");
		udp_h = (struct udphdr *)skb_transport_header(skb);
		data_ptr = skb->data + sizeof(struct iphdr) + sizeof(struct udphdr);
		ptr->ttl = *(int *)data_ptr;
		if(ptr->ttl-- == 0) return NF_DROP;
		printk(KERN_ALERT " ptr->ttl : %x \n", ptr->ttl);
		memmove((void *)(data_ptr + SIZE),(void *)data_ptr, skb->len);
		memcpy((void *)data_ptr, ptr, SIZE);
		data_len = (unsigned int)ntohs(udp_h->len);
		udp_h->len = htons(data_len + SIZE);
		skb->tail += SIZE;
		skb->len += SIZE; 
		udp_h->check = 0;
		offset = skb_transport_offset(skb);
		length = skb->len - offset;
		skb->csum = skb_checksum(skb, offset, length, 0);
		udp_h->check = csum_tcpudp_magic((ip_h->saddr), (ip_h->daddr), 
						  length, UDP, csum_partial((unsigned char*)udp_h,length,0));
		data_len = (unsigned int)ntohs(ip_h->tot_len);
		ip_h->tot_len = htons(data_len + SIZE);
		ip_h->check = 0;
		ip_h->check = ip_fast_csum((unsigned char*)ip_h, ip_h->ihl);
		break;
	default:
		break;    
    }
  }
  kfree(ptr);
  return NF_ACCEPT;
}


//Called when module loaded using 'insmod'
int init_module()
{
  printk(KERN_ALERT "\n +++++++++ Initialize Routing Kernel Module +++++++++\n ");

  nf_forw.hook = forwarding_hook_func;
  nf_forw.hooknum = NF_INET_FORWARD;
  nf_forw.pf = PF_INET;
  nf_forw.priority = NF_IP_PRI_FIRST;
  nf_register_hook(&nf_forw);

  return 0;   
}

//Called when module unloaded using 'rmmod'
void cleanup_module()
{                   
  nf_unregister_hook(&nf_forw);
  printk(KERN_ALERT "\n -------- Routing Kernel Module Unloaded --------- \n");
}