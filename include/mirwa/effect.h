#ifndef MIRWA_EFFECT_H
#define MIRWA_EFFECT_H

#include <epilepsy.h>

#include <mirwa/aux.h>
#include <mirwa/unit.h>

#define EXPAND(...)       __VA_ARGS__
#define UNPARENTHESISE(x) EXPAND(EXPAND x)

#define resume(...)                                                                                \
    *mirwa_priv_handler_exec_point = __LINE__;                                                     \
    return __VA_ARGS__;                                                                            \
    case __LINE__:

#define Effect(...)    EPILEPSY_EVAL(EPILEPSY_OVERLOAD_CALL(v(Effect_), v(__VA_ARGS__)))
#define Effect_1(name) Effect_2(name, { MirwaUnit mirwa_priv_dummy; })
#define Effect_2(name, state)                                                                      \
    v(typedef struct name##EffectState state name##EffectState; const MirwaUnit name##Effect)

#define EffectHandler(effect, return_ty, op, params, ...)                                          \
    struct effect##Effect_##op##_Args {                                                            \
        MirwaUnit mirwa_priv_dummy;                                                                \
        UNPARENTHESISE(params);                                                                    \
    };                                                                                             \
                                                                                                   \
    return_ty effect##Effect_##op(                                                                 \
        int mirwa_priv_handler_exec_point[const restrict MIRWA_NON_NULL],                          \
        effect##EffectState *self, struct effect##Effect_##op##_Args args) {                       \
        (void)self;                                                                                \
        switch (*mirwa_priv_handler_exec_point) {                                                  \
        case 0:                                                                                    \
            __VA_ARGS__ *mirwa_priv_handler_exec_point = -1;                                       \
        }                                                                                          \
    }                                                                                              \
                                                                                                   \
    typedef struct effect##Effect_##op##_Args effect##Effect_##op##_Args

#define performEffect(effect, op, args, ...)                                                       \
    MirwaHandlers *mirwa_priv_stack = &mirwa_handlers;                                             \
    while (mirwa_priv_stack->effect_discriminant != &effect##Effect) {                             \
        mirwa_priv_stack = (MirwaHandlers *)mirwa_priv_stack->prev;                                \
    }                                                                                              \
                                                                                                   \
    int mirwa_priv_handler_exec_point = 0;                                                         \
    while (1) {                                                                                    \
        effect##Effect##_##op(                                                                     \
            &mirwa_priv_handler_exec_point, (effect##EffectState *)mirwa_priv_stack->effect_state, \
            (effect##Effect##_##op##_Args){mirwa_unit, UNPARENTHESISE(args)});                     \
                                                                                                   \
        if (mirwa_priv_handler_exec_point == -1) {                                                 \
            break;                                                                                 \
        }                                                                                          \
                                                                                                   \
        __VA_ARGS__                                                                                \
    }                                                                                              \
                                                                                                   \
    do {                                                                                           \
    } while (0)

typedef struct MirwaHandlers {
    struct MirwaHandlers *prev;
    const void *effect_discriminant;
    void *effect_state;
} MirwaHandlers;

#define handle_with(effect)                                                                        \
    mirwa_handlers = (MirwaHandlers) {                                                             \
        .prev = MIRWA_OBJ(MirwaHandlers, mirwa_handlers), .effect_discriminant = &effect##Effect,  \
        .effect_state = &(effect##EffectState){0},                                                 \
    }

#endif // MIRWA_EFFECT_H
