/*
 * =====================================================================================
 *
 *       Filename:  ddcp.h
 *
 *    Description:  This file implements the definition of structures and declaratio ofpublic APIs related to DDCP (Distributed Data Collection Protocol)
 *
 *        Version:  1.0
 *        Created:  11/09/2019 02:07:34 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Er. Abhishek Sagar, Juniper Networks (https://csepracticals.wixsite.com/csepracticals), sachinites@gmail.com
 *        Company:  Juniper Networks
 *
 *        This file is part of the DDCP distribution (https://github.com/sachinites) 
 *        Copyright (c) 2019 Abhishek Sagar.
 *        This program is free software: you can redistribute it and/or modify it under the terms of the GNU General 
 *        Public License as published by the Free Software Foundation, version 3.
 *        
 *        This program is distributed in the hope that it will be useful, but
 *        WITHOUT ANY WARRANTY; without even the implied warranty of
 *        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *        General Public License for more details.
 *
 *        visit website : https://csepracticals.wixsite.com/csepracticals for more courses and projects
 *                                  
 * =====================================================================================
 */

#ifndef __DDCP__
#define __DDCP__

#include "../../utils.h"
#include "../../graph.h"
#include "../../Layer2/layer2.h"

typedef enum{

    DDCP_TLV_RTR_NAME,
    DDCP_TLV_RTR_LO_ADDR,
    DDCP_TLV_RAM_SIZE,
    DDCP_TLV_OS_VERSION,
    DDCP_TLV_MAX
} DDCP_TLV_ID;

static inline char *ddcp_tlv_id_str(
    DDCP_TLV_ID ddcp_tlv_id){

    switch(ddcp_tlv_id){
        case DDCP_TLV_RTR_NAME:
            return "DDCP_TLV_RTR_NAME";
        case DDCP_TLV_RTR_LO_ADDR:
            return "DDCP_TLV_RTR_LO_ADDR";
        case DDCP_TLV_RAM_SIZE:
            return "DDCP_TLV_RAM_SIZE";
        case DDCP_TLV_OS_VERSION:
            return "DDCP_TLV_OS_VERSION";
        case DDCP_TLV_MAX:
            return "DDCP_TLV_MAX";
        default:    
            return "DDCP_TLV_UNKNOWN";
    }
}

typedef struct ddcp_query_hdr_{
    unsigned int originator_ip;
    unsigned int seq_no;
    unsigned int no_of_tlvs;
    DDCP_TLV_ID tlv_code_points[0];
} ddcp_query_hdr_t;

typedef struct ddcp_interface_prop_{

    bool_t is_enabled; 
} ddcp_interface_prop_t;


void
init_ddcp_interface_props(ddcp_interface_prop_t **ddcp_interface_prop);

static inline bool_t
is_interface_ddcp_enabled(ddcp_interface_prop_t *ddcp_interface_prop){

    return ddcp_interface_prop->is_enabled;
}

void
ddcp_send_ddcp_query_out(char *pkt, 
                         unsigned int pkt_size, 
                         interface_t *oif);

void
ddcp_flood_ddcp_query_out(node_t *node, char *pkt, 
                          unsigned int pkt_size,
                          interface_t *exempted_intf);

void
ddcp_process_ddcp_query_msg(node_t *node, interface_t *iif,
                              ethernet_hdr_t *ethernet_hdr,
                              unsigned int pkt_size);

void
ddcp_process_ddcp_reply_msg(node_t *node, char *pkt);


/*DDCP Query Database*/
typedef unsigned int seq_t;

typedef struct ddcp_db_query_node_{

    unsigned int originator_ip;
    seq_t seq_no;
    glthread_t ddcp_db_query_node_glue;
} ddcp_db_query_node_t;
GLTHREAD_TO_STRUCT(ddcp_db_query_node_glue_to_ddcp_db_query_node, 
                    ddcp_db_query_node_t, ddcp_db_query_node_glue);

typedef struct ddcp_reply_msg_{

    glthread_t glue;
    char reply_msg[0];
} ddcp_reply_msg_t;

GLTHREAD_TO_STRUCT(ddcp_db_reply_node_glue_to_ddcp_reply_msg,
                    ddcp_reply_msg_t, glue);

typedef struct ddcp_db_{
    glthread_t ddcp_query_head;
    glthread_t ddcp_reply_head;
} ddcp_db_t;

void
init_ddcp_query_db(ddcp_db_t **ddcp_db);

bool_t 
ddcp_db_should_process_ddcp_query(node_t *node, 
                                  unsigned int originator_ip,
                                  seq_t seq_no);

seq_t
ddcp_update_ddcp_db_self_query_info(node_t *node);

void
ddcp_print_ddcp_reply_msgs_db(node_t *node);

#define GET_NODE_DDCP_DB(node_ptr)  \
    (node_ptr->node_nw_prop.ddcp_db)

#define GET_NODE_DDCP_DB_HEAD(node_ptr) \
    (&(GET_NODE_DDCP_DB(node)->ddcp_query_head))

#define GET_NODE_DDCP_DB_REPLY_HEAD(node_ptr) \
    (&(GET_NODE_DDCP_DB(node)->ddcp_reply_head))

/*DDCP reply msg management*/
#define GET_SEQ_NO(pkt_ptr)     (*(seq_t *)pkt_ptr)
#define GET_PKT_TLEN(pkt_ptr)   (*(unsigned int *)(pkt_ptr + sizeof(seq_t)))
#define DDCP_AUX_INFO           ((sizeof(seq_t) + sizeof(unsigned int)))
#define TLV_SIZE(pkt_ptr)       ((unsigned int)(GET_PKT_TLEN(pkt_ptr) - DDCP_AUX_INFO))
#define GET_TLV_START_PTR(pkt_ptr) (pkt_ptr + DDCP_AUX_INFO)
#endif /*__DDCP__*/