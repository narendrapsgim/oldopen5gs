#define TRACE_MODULE _esm_sm

#include "core_debug.h"

#include "nas_message.h"

#include "mme_event.h"
#include "esm_handler.h"
#include "esm_build.h"

void esm_state_initial(fsm_t *s, event_t *e)
{
    d_assert(s, return, "Null param");

    mme_sm_trace(3, e);

    FSM_TRAN(s, &esm_state_operational);
}

void esm_state_final(fsm_t *s, event_t *e)
{
    d_assert(s, return, "Null param");

    mme_sm_trace(3, e);
}

void esm_state_operational(fsm_t *s, event_t *e)
{
    d_assert(s, return, "Null param");
    d_assert(e, return, "Null param");

    mme_sm_trace(3, e);

    switch (event_get(e))
    {
        case FSM_ENTRY_SIG:
        {
            break;
        }
        case FSM_EXIT_SIG:
        {
            break;
        }
        case MME_EVT_ESM_BEARER_MSG:
        {
            index_t index = event_get_param1(e);
            mme_bearer_t *bearer = NULL;
            mme_ue_t *mme_ue = NULL;
            nas_message_t *message = NULL;

            d_assert(index, return, "Null param");
            bearer = mme_bearer_find(index);
            d_assert(bearer, return, "Null param");
            mme_ue = bearer->mme_ue;
            d_assert(mme_ue, return, "Null param");
            message = (nas_message_t *)event_get_param4(e);
            d_assert(message, break, "Null param");

            /* Save Last Received NAS-ESM message */
            memcpy(&mme_ue->last_esm_message, message, sizeof(nas_message_t));

            switch(message->esm.h.message_type)
            {
                case NAS_PDN_CONNECTIVITY_REQUEST:
                {
                    esm_handle_pdn_connectivity_request(
                            bearer, &message->esm.pdn_connectivity_request);
                    d_trace(3, "[NAS] PDN connectivity request : "
                            "UE[%s] --> ESM[%d]\n", 
                            mme_ue->imsi_bcd, bearer->pti);
                    break;
                }
                case NAS_ESM_INFORMATION_RESPONSE:
                {
                    /* FIXME : SGW Selection */
                    bearer->sgw = mme_sgw_first();

                    d_trace(3, "[NAS] ESM information response : "
                            "UE[%s] --> ESM[%d]\n", 
                            mme_ue->imsi_bcd, bearer->pti);
                    esm_handle_information_response(
                            bearer, &message->esm.esm_information_response);
                    break;
                }
                case NAS_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_ACCEPT:
                {
                    d_trace(3, "[NAS] Activate default eps bearer "
                            "context accept : UE[%s] --> ESM[%d]\n", 
                            mme_ue->imsi_bcd, bearer->pti);
                    break;
                }
                default:
                {
                    d_warn("Not implemented(type:%d)", 
                            message->esm.h.message_type);
                    break;
                }
            }
            break;
        }

        default:
        {
            d_error("Unknown event %s", mme_event_get_name(e));
            break;
        }
    }
}

void esm_state_exception(fsm_t *s, event_t *e)
{
    d_assert(s, return, "Null param");
    d_assert(e, return, "Null param");

    mme_sm_trace(3, e);

    switch (event_get(e))
    {
        case FSM_ENTRY_SIG:
        {
            break;
        }
        case FSM_EXIT_SIG:
        {
            break;
        }
        default:
        {
            d_error("Unknown event %s", mme_event_get_name(e));
            break;
        }
    }
}
