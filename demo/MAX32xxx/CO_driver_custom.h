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

/* Override eeprom size in bytes, 0x8000 for 25LC256 */
//#define CO_STOR_EE_SIZE 0x8000

/* If eeprom is not present, disable storage */
//#define CO_CONFIG_STORAGE (0)

/* Add parameters for application specified storage */
#define CO_STORAGE_APPLICATION \
    { \
        .addr = &OD_PERSIST_APP, \
        .len = sizeof(OD_PERSIST_APP), \
        .subIndexOD = 5, \
        .attr = CO_storage_cmd | CO_storage_restore \
    }, \
    { \
        .addr = &OD_PERSIST_APP_AUTO, \
        .len = sizeof(OD_PERSIST_APP_AUTO), \
        .subIndexOD = 6, \
        .attr = CO_storage_cmd | CO_storage_auto | CO_storage_restore \
    }

/* Specify optional OD entry for CO_CANopenInit() -> CO_EM_init() */
#define OD_STATUS_BITS OD_ENTRY_H2100_errorStatusBits

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CO_DRIVER_CUSTOM_H */
