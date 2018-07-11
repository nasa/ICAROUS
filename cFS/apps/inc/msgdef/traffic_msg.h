/**
 * @file traffic_msg.h
 * @brief Traffic message definition
 */
#ifndef ICAROUS_CFS_TRAFFIC_MSG_H
#define ICAROUS_CFS_TRAFFIC_MSG_H

#include <stdint.h>
#include "cfe.h"
#include "stdbool.h"


/**
 * @defgroup TRAFFIC_MESSAGES
 * @brief Definition of messages generated by the traffic application
 * @ingroup TRAFFIC
 * @ingroup MESSAGES
 * @{
 */


/**
 * @struct bands_t
 * @brief kinematics bands
 *
 * Published under the following topics: ICAROUS_BANDS_TRACK_MID, ICAROUS_BANDS_SPEED_MID, ICAROUS_BANDS_VS_MID
 */
typedef struct{
    uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE];  /**< cFS header information */
    int numConflictTraffic;                  /**< total number of conflict traffic */
	int numBands;                            /**< total number of bands (max:5)*/
	int type[20];                            /**< type of each band @see Region */
	double min[20];                          /**< min values of each band */
	double max[20];                          /**< max values of each band */
    int recovery;                            /**< 1 if recovery band is available */
	int currentConflictBand;                 /**< current ownship conflict band */
	double timeToViolation;                  /**< time to violation */
	double timeToRecovery;                   /**< time to recovery */
	double minHDist;                         /**< min horizontal distance */
	double minVDist;                         /**< min vertical distance */
	double resUp;                            /**< resolution up */
	double resDown;                          /**< resolution down */
	double resPreferred;                     /**< preferred resolution */
    bool wpFeasibility[50];                  /**< feasiblity to waypoints */
}bands_t;

/**
 * @struct checkTurnSafety_t
 * @brief Message used to request/respond to check if a turn from heading A to heading B is safe.
 *
 * A request is published under TRAFFIC_TURN_CHECK_MID.
 * A response to the above query is also sent in this message published in the TRAFFIC_TURN_CHECK_RESPONSE_MID.
 */
typedef struct{
   uint8_t	TlmHeader[CFE_SB_TLM_HDR_SIZE]; /**< cFS header information */
   double fromHeading;                      /**< from heading (degree) */
   double toHeading;	                    /**< to heading (degree)*/
   bool safe;                               /**< true if safe to turn (only used in the response) */
}checkTurnSafety_t;

/**
 * @enum Region
 * Defines the band types
 */
enum Region {UNKNOWN,NONE,RECOVERY,NEAR,MID,FAR};


/**@}*/
#endif //ICAROUS_CFS_TRAFFIC_MSG_H