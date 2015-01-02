#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/slab.h>
#include <linux/inet.h>
#include <linux/time.h>

#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <linux/seq_file.h>
#include <asm/uaccess.h>

#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/icmp.h>

static struct nf_hook_ops nf_out;
static struct nf_hook_ops nf_in;

struct loc_time_tag
{
  int  ttl;
  int latitude;
  int longitude;
  unsigned int timestamp;
};

#define MAX_HOPS 0x04
#define ICMP     0x01
#define UDP      0x11
#define TCP      0x06
#define SIZE sizeof(struct loc_time_tag)

static char ttl_buf[MAX_HOPS - 1][5];
static char time_buf[MAX_HOPS -1][50];
static char lat_buf[MAX_HOPS -1][25];
static char long_buf[MAX_HOPS -1][25];
static int hop_cnt;


static struct proc_dir_entry *proc_entry;
static char *info;


static int geo_proc_show(struct seq_file *m, void *v) {
  int i;
  seq_printf(m,"Info collected from the incoming packets:\n");
  for(i = 0; i <= hop_cnt; i++) 
 {
   seq_printf(m," TTL: %s\n Latitude: %s\n Longitude: %s\n Time:%s\n\n\n", ttl_buf[i], lat_buf[i], long_buf[i], time_buf[i]);
 }
  return 0;
}

static int geo_proc_open(struct inode *inode, struct  file *file) {
  return single_open(file, geo_proc_show, NULL);
}

static const struct file_operations geo_proc_fops = {
  .owner = THIS_MODULE,
  .open = geo_proc_open,
  .read = seq_read,
  .llseek = seq_lseek,
  .release = single_release,
};

struct timeval outgoingtime(void)
{
  struct timeval currtime;
  do_gettimeofday(&currtime);
  return currtime;
}

void incomingtime(unsigned int incoming, char *str)
{
  time_t time;
  struct tm rcv;
  struct timeval sent_time;
  sent_time.tv_sec = incoming;
  time = (time_t)sent_time.tv_sec;
  time_to_tm(time,0,&rcv);
  
  printk("%04d/%02d/%02d %02d:%02d:%02d (UTC) \n", rcv.tm_year+1900,rcv.tm_mon+1,rcv.tm_mday, rcv.tm_hour,rcv.tm_min,rcv.tm_sec);
  
  snprintf(str, 50, "%04d/%02d/%02d %02d:%02d:%02d (UTC) \n", rcv.tm_year+1900,rcv.tm_mon+1,rcv.tm_mday, rcv.tm_hour,rcv.tm_min,rcv.tm_sec);
}

int outgoingCoord(float a)
{
  int degree = (int)a;
  if(degree < 0)
  {
    a = ((float)degree - a) * 60;
  }
  else
  {
    a = (a - (float)degree) * 60;
  }
  return (int)(((degree*60)+a) * 30000);
}

void incomingCoord(int a, char* str)
{
  float b;
  int degree, minutes, seconds;

  b = (float)a / 30000;
  degree = (int)b / 60;
  if(a < 0)
  {
    degree--;
  }
  b = b - degree*60;
  minutes = (int)b;
  b = (b-minutes)*60;
  seconds = (int)b;
  printk("%d^ %d' %d'' \n",degree,minutes,seconds);
  snprintf(str, 25, "%d^ %d' %d'', ",degree,minutes,seconds);
}

void print_ip(unsigned int ipaddr, char *str)
{
        unsigned char a[4] = {0,0,0,0};
        int i;
        for (i = 0; i < 4; i++)
            a[i] = (ipaddr >> (i*8)) & 0xff;
        snprintf(str, 15, "%d.%d.%d.%d, ", a[0], a[1], a[2], a[3]);
}

unsigned int outgoing_hook_func(unsigned int hooknum, struct sk_buff *skb, const struct net_device *in, const struct net_device *out, int(*okfn)(struct sk_buff *))
{
  struct timeval currtime;
  struct loc_time_tag *ptr;
  struct iphdr *ip_h;
  struct udphdr *udp_h;
  unsigned char *data_ptr;
  unsigned int data_len;
  int length;
  int offset;

  currtime = outgoingtime();
  ptr = kmalloc(SIZE, GFP_ATOMIC);
  ptr->ttl = MAX_HOPS;
  ptr->latitude  = outgoingCoord(35.769877); 
  ptr->longitude = outgoingCoord(-78.675131); 
  ptr->timestamp = (u32)currtime.tv_sec;
  if((skb->tail+(MAX_HOPS*SIZE)) < skb->end && (!skb_is_nonlinear(skb)))
  {
    printk(KERN_ALERT "\n   Outgoing_hook_func @ work **/ \n");
    ip_h = (struct iphdr *)skb_network_header(skb);
    switch(ip_h->protocol)
    {
	case TCP:
		printk(KERN_ALERT " TCP PACKET OUT \n");	  
		break;
	case UDP:		
		printk(KERN_ALERT " UDP PACKET OUT \n");
		udp_h = (struct udphdr *)skb_transport_header(skb);
		data_ptr = skb->data + sizeof(struct iphdr) + sizeof(struct udphdr);
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
		ip_h->tos |= 0x7C;
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

unsigned int incoming_hook_func(unsigned int hooknum, struct sk_buff *skb, const struct net_device *in, const struct net_device *out, int(*okfn)(struct sk_buff *))
{
  struct iphdr *ip_h;
  struct udphdr *udp_h;
  struct loc_time_tag *ptr, *temp;
  unsigned char *data_ptr;
  unsigned int data_len;
  unsigned int i;
  unsigned int length;
  unsigned int offset;
  unsigned int head_size;
 
    printk(KERN_ALERT "\n Incoming_hook_func @ work :: MY PACKET **/ \n");
    ip_h = (struct iphdr *)skb_network_header(skb);
    if(ip_h->tos & 0x7C)
    {
      switch(ip_h->protocol)
      {
	case TCP:
	  break;
	case UDP:		
		printk(KERN_ALERT " UDP PACKET IN \n");
		udp_h = (struct udphdr *)skb_transport_header(skb);
		data_ptr = skb->data + sizeof(struct iphdr) + sizeof(struct udphdr);
		ptr = (struct loc_time_tag *)data_ptr;
		head_size = MAX_HOPS - ptr->ttl;
		temp = ptr;
		hop_cnt = head_size;
		for (i=0; i <= head_size; i++)
		{
		      printk(KERN_ALERT " TTL: %d \n",temp->ttl);
		      snprintf(ttl_buf[i], 5, "%d, ", i);
		      printk(KERN_ALERT " LATITUDE: ");incomingCoord(temp->latitude,lat_buf[i]);
		      printk(KERN_ALERT " LONGITUDE: ");incomingCoord(temp->longitude,long_buf[i]);
		      printk(KERN_ALERT " TIMESTAMP: ");incomingtime(temp->timestamp, time_buf[i]);
		      temp++;
		}
		if(ptr->ttl-- == 0) 
		  return NF_DROP;
		head_size = MAX_HOPS - ptr->ttl;
		memmove((void*)data_ptr,(void*)(data_ptr+(SIZE*head_size)),(skb->len-(SIZE*head_size)));
		data_len = (unsigned int)ntohs(ip_h->tot_len);
		ip_h->tot_len = htons(data_len - (SIZE*head_size));
		ip_h->check = 0;
		ip_h->check = ip_fast_csum((unsigned char*)ip_h, ip_h->ihl);
		skb->tail -= (SIZE * head_size);
		skb->len -= (SIZE * head_size); 
		data_len = (unsigned int)ntohs(udp_h->len);
		udp_h->len = htons(data_len - (SIZE*head_size));
		udp_h->check = 0;
		offset = skb_transport_offset(skb);
		length = skb->len - offset;
		skb->csum = skb_checksum(skb, offset, length, 0);
		udp_h->check = csum_tcpudp_magic((ip_h->saddr), (ip_h->daddr), 
						  length, UDP, csum_partial((unsigned char*)udp_h,length,0));
		break;
	default:
		break;
      }
    }
  return NF_ACCEPT;
}

//Called when module loaded using 'insmod'
int init_module()
{
  printk(KERN_ALERT "\n +++++++++ Initialize Client Kernel Module +++++++++\n ");

  nf_in.hook = incoming_hook_func;       //function to call when conditions below met
  nf_in.hooknum = NF_INET_PRE_ROUTING;   //called right after packet recieved, first hook in Netfilter
  nf_in.pf = PF_INET;                    //IPV4 packets
  nf_in.priority = NF_IP_PRI_FIRST;      //set to highest priority over all other hook functions
  nf_register_hook(&nf_in);              //register hook
  
  
  nf_out.hook = outgoing_hook_func;      //function to call when conditions below met
  nf_out.hooknum = NF_INET_POST_ROUTING; //called right after packet recieved, first hook in Netfilter
  nf_out.pf = PF_INET;                   //IPV4 packets
  nf_out.priority = NF_IP_PRI_FIRST;     //set to highest priority over all other hook functions
  nf_register_hook(&nf_out);             //register hook
  
  proc_create("geo_proc", 0, NULL, &geo_proc_fops);

  return 0;   
}

//Called when module unloaded using 'rmmod'
void cleanup_module()
{
  nf_unregister_hook(&nf_in);                     
  nf_unregister_hook(&nf_out);
  printk(KERN_ALERT "\n -------- Client Kernel Module Unloaded --------- \n");
  remove_proc_entry("geo_proc", proc_entry);
  printk(KERN_INFO "geo_proc unloaded.\n");
  vfree(info);
}