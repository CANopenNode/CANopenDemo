/*
 * Application interface for CANopenNode.
 *
 * @file        CO_application.c
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


#include "CO_application.h"
#include "OD.h"
#include "peripheral.h"
#include "CO_identificators.h"
#include "objectAccessOD.h"
#include "domainDemo.h"

/* Define object for objectAccessOD */
objectAccessOD_t objectAccessOD;

/* Variables used for triggering TPDO, see simulation in app_programRt(). */
OD_extension_t OD_variableInt32_extension = {
    .object = NULL,
    .read = OD_readOriginal,
    .write = OD_writeOriginal
};
uint8_t *OD_variableInt32_flagsPDO = NULL;


/******************************************************************************/
CO_ReturnError_t app_programStart(uint16_t *bitRate,
                                  uint8_t *nodeId,
                                  uint32_t *errInfo)
{
    CO_ReturnError_t err = CO_ERROR_NO;

    /* Target device specific initialization */
    err = peripheral_init();
    if (err != CO_ERROR_NO)
        return err;

    /* increment auto-storage variable */
    OD_PERSIST_APP_AUTO.x2106_power_onCounter ++;

    /* Setup bitRate, nodeId and OD objects 0x1008, 0x1009, 0x100A and 0x1018 */
    CO_identificators_init(bitRate, nodeId);

    /* Setup extension and flags for triggering TPDO. */
    OD_extension_init(OD_ENTRY_H2110_variableInt32,
                      &OD_variableInt32_extension);
    OD_variableInt32_flagsPDO = OD_getFlagsPDO(OD_ENTRY_H2110_variableInt32);

    /* Initialize more advanced object, which operates with Object Dictionary
     * variables in
     * OD_ENTRY_H2120_demoObject is constant defined in OD.h. More
     * flexible alternative for third argument is 'OD_find(&OD, 0x2120)' */
    err = objectAccessOD_init(&objectAccessOD,
                              OD_ENTRY_H2120_demoRecord,
                              errInfo);
    if (err != CO_ERROR_NO)
        return err;

    domainDemo_init(OD_ENTRY_H2122_demoDomain, errInfo);
    return err;
}


/******************************************************************************/
void app_communicationReset(CO_t *co) {

    /* example printouts */
    if (!co->nodeIdUnconfigured) {
        /* CANopen Node-ID is configured and all services will work. */
    }
    else {
        /* CANopen Node-ID is unconfigured, so only LSS slave will work. */
    }
}


/******************************************************************************/
void app_programEnd() {

}


/******************************************************************************/
void app_programAsync(CO_t *co, uint32_t timer1usDiff) {
    (void) co; (void) timer1usDiff; /* unused */
}


/******************************************************************************/
void app_programRt(CO_t *co, uint32_t timer1usDiff) {
    (void) co; (void) timer1usDiff; /* unused */

    /* Simulation: detect change of state of the variable and trigger TPDO, to
     * which variable is possibly mapped. In our example x2110_variableInt32[0]
     * variable will be mapped to TPDO. If program detects change-of-state of
     * its value, TPDO will be triggered for sending. (x2110_variableInt32[0]
     * variable can be changed by SDO.)
     */

    /* static variable is like global: initialized to 0, then keeps its value */
    static int32_t value_old = 0;
    int32_t value_current = OD_RAM.x2110_variableInt32[0];

    /* Detect change of state and trigger TPDO. Of course, variable must be
     * mapped to event driven TPDO for this to have effect. */
    if (value_current != value_old) {
        OD_requestTPDO(OD_variableInt32_flagsPDO, 1); /* subindex is 1 */
    }

    value_old = value_current;
}
