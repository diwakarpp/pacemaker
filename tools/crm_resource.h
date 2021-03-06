/*
 * Copyright 2004-2018 Andrew Beekhof <andrew@beekhof.net>
 *
 * This source code is licensed under the GNU Lesser General Public License
 * version 2.1 or later (LGPLv2.1+) WITHOUT ANY WARRANTY.
 */

#include <crm_internal.h>
#include <crm/crm.h>

#include <crm/msg_xml.h>
#include <crm/services.h>
#include <crm/common/xml.h>
#include <crm/common/mainloop.h>

#include <crm/cib.h>
#include <crm/attrd.h>
#include <crm/pengine/rules.h>
#include <crm/pengine/status.h>
#include <crm/pengine/internal.h>
#include <pacemaker-internal.h>

extern bool print_pending;

extern bool scope_master;
extern bool do_force;
extern bool BE_QUIET;
extern int resource_verbose;

extern int cib_options;
extern int crmd_replies_needed;

extern char *move_lifetime;

extern const char *attr_set_type;

/* ban */
int cli_resource_prefer(const char *rsc_id, const char *host, cib_t * cib_conn);
int cli_resource_ban(const char *rsc_id, const char *host, GListPtr allnodes, cib_t * cib_conn);
int cli_resource_clear(const char *rsc_id, const char *host, GListPtr allnodes, cib_t * cib_conn,
                       bool clear_ban_constraints);
int cli_resource_clear_all_expired(xmlNode *root, cib_t *cib_conn, const char *rsc, const char *node, bool scope_master);

/* print */
void cli_resource_print_cts(resource_t * rsc);
void cli_resource_print_raw(resource_t * rsc);
void cli_resource_print_cts_constraints(pe_working_set_t * data_set);
void cli_resource_print_location(resource_t * rsc, const char *prefix);
void cli_resource_print_colocation(resource_t * rsc, bool dependents, bool recursive, int offset);

int cli_resource_print(resource_t *rsc, pe_working_set_t *data_set,
                       bool expanded);
int cli_resource_print_list(pe_working_set_t * data_set, bool raw);
int cli_resource_print_attribute(resource_t *rsc, const char *attr,
                                 pe_working_set_t *data_set);
int cli_resource_print_property(resource_t *rsc, const char *attr,
                                pe_working_set_t *data_set);
int cli_resource_print_operations(const char *rsc_id, const char *host_uname, bool active, pe_working_set_t * data_set);

/* runtime */
void cli_resource_check(cib_t * cib, resource_t *rsc);
int cli_resource_fail(crm_ipc_t * crmd_channel, const char *host_uname, const char *rsc_id, pe_working_set_t * data_set);
int cli_resource_search(resource_t *rsc, const char *requested_name,
                        pe_working_set_t *data_set);
int cli_resource_delete(crm_ipc_t *crmd_channel, const char *host_uname,
                        resource_t *rsc, const char *operation,
                        const char *interval_spec, bool just_failures,
                        pe_working_set_t *data_set);
int cli_cleanup_all(crm_ipc_t *crmd_channel, const char *node_name,
                    const char *operation, const char *interval_spec,
                    pe_working_set_t *data_set);
int cli_resource_restart(pe_resource_t *rsc, const char *host, int timeout_ms,
                         cib_t *cib);
int cli_resource_move(resource_t *rsc, const char *rsc_id,
                      const char *host_name, cib_t *cib,
                      pe_working_set_t *data_set);
int cli_resource_execute_from_params(const char *rsc_name, const char *rsc_class,
                                     const char *rsc_prov, const char *rsc_type,
                                     const char *rsc_action, GHashTable *params,
                                     GHashTable *override_hash, int timeout_ms);
int cli_resource_execute(resource_t *rsc, const char *requested_name,
                         const char *rsc_action, GHashTable *override_hash,
                         int timeout_ms, cib_t *cib,
                         pe_working_set_t *data_set);

int cli_resource_update_attribute(resource_t *rsc, const char *requested_name,
                                  const char *attr_set, const char *attr_id,
                                  const char *attr_name, const char *attr_value,
                                  bool recursive, cib_t *cib,
                                  pe_working_set_t *data_set);
int cli_resource_delete_attribute(resource_t *rsc, const char *requested_name,
                                  const char *attr_set, const char *attr_id,
                                  const char *attr_name, cib_t *cib,
                                  pe_working_set_t *data_set);

GList* subtract_lists(GList *from, GList *items, GCompareFunc cmp);

int update_working_set_xml(pe_working_set_t *data_set, xmlNode **xml);
int wait_till_stable(int timeout_ms, cib_t * cib);
void cli_resource_why(cib_t *cib_conn, GListPtr resources, resource_t *rsc,
                      node_t *node);

extern xmlNode *do_calculations(pe_working_set_t * data_set, xmlNode * xml_input, crm_time_t * now);
