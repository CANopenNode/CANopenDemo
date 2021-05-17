/*
 * Custom definitions for CANopenNode.
 *
 * @file        CO_driver_custom.h
 * @author      --
 * @copyright   2021 --
 *
 * This file is part of CANopenNode, an opensource CANopen Stack.
 * Project home page is <https://github.com/CANopenNode/CANopenNode>.
 * For more information on CANopen see <http://www.can-cia.org/>.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CO_DRIVER_CUSTOM_H
#define CO_DRIVER_CUSTOM_H

#ifdef __cplusplus
extern "C" {
#endif

/* This file contains application specific custom definitions for CANopenNode.
 * It is included from CO_driver_target.h, so it may override any default
 * setting. See also file CO_config.h. */

/* Include CO_application.h interface in the mainline */
#define CO_USE_APPLICATION


/* Disable gateway functions, which are enabled in CO_driver_target.h. */
#ifndef CO_GATEWAY
/* without CO_CONFIG_EM_CONSUMER */
#define CO_CONFIG_EM (CO_CONFIG_EM_PRODUCER | \
                      CO_CONFIG_EM_PROD_CONFIGURABLE | \
                      CO_CONFIG_EM_PROD_INHIBIT | \
                      CO_CONFIG_EM_HISTORY | \
                      CO_CONFIG_EM_STATUS_BITS | \
                      CO_CONFIG_FLAG_CALLBACK_PRE | \
                      CO_CONFIG_FLAG_TIMERNEXT)

/* Gateway command interface, SDO client and LSS master are disabled. */
#define CO_CONFIG_SDO_CLI (0)

/* without CO_CONFIG_LSS_MASTER */
#define CO_CONFIG_LSS (CO_CONFIG_LSS_SLAVE | \
                       CO_CONFIG_LSS_SLAVE_FASTSCAN_DIRECT_RESPOND | \
                       CO_CONFIG_FLAG_CALLBACK_PRE)

#define CO_CONFIG_GTW (0)

#define CO_CONFIG_FIFO (0)
#endif /* CO_GATEWAY */


/* Add parameters for application specified storage */
#define CO_STORAGE_APPLICATION \
    { \
        .addr = &OD_PERSIST_APP, \
        .len = sizeof(OD_PERSIST_APP), \
        .subIndexOD = 5, \
        .attr = CO_storage_cmd | CO_storage_restore, \
        .filename = {'o','d','_','a','p','p', \
                     '.','p','e','r','s','i','s','t','\0'} \
    }, \
    { \
        .addr = &OD_PERSIST_APP_AUTO, \
        .len = sizeof(OD_PERSIST_APP_AUTO), \
        .subIndexOD = 6, \
        .attr = CO_storage_cmd | CO_storage_auto | CO_storage_restore, \
        .filename = {'o','d','_','a','p','p','_','a','u','t','o', \
                     '.','p','e','r','s','i','s','t','\0'} \
    }


/* Specify optional OD entry for CO_CANopenInit() -> CO_EM_init() */
#define OD_STATUS_BITS OD_ENTRY_H2100_errorStatusBits

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CO_DRIVER_CUSTOM_H */
