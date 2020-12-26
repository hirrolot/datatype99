#ifndef MIRWA_EFFECT_H
#define MIRWA_EFFECT_H

#include <epilepsy.h>

#include <mirwa/aux.h>
#include <mirwa/unit_t.h>

#define EXPAND(...)       __VA_ARGS__
#define UNPARENTHESISE(x) EXPAND(EXPAND x)

#define Effect(...)    EPILEPSY_EVAL(EPILEPSY_OVERLOAD_CALL(v(Effect_), v(__VA_ARGS__)))
#define Effect_1(name) Effect_2(name, { MirwaUnitT mirwa_priv_dummy; })
#define Effect_2(name, state)                                                                      \
    v(typedef struct name##EffectState state name##EffectState; const MirwaUnitT name##Effect)

#define EffectHandler(effect, return_ty, op, params, ...)                                          \
    struct effect##Effect_##op##_Args {                                                            \
        MirwaUnitT mirwa_priv_dummy;                                                               \
        UNPARENTHESISE(params);                                                                    \
    };                                                                                             \
                                                                                                   \
    return_ty effect##Effect_##op(                                                                 \
        int mirwa_priv_handler_exec_point[const restrict MIRWA_NON_NULL],                          \
        effect##EffectState self[restrict MIRWA_NON_NULL],                                         \
        struct effect##Effect_##op##_Args args) {                                                  \
        (void)self;                                                                                \
        switch (*mirwa_priv_handler_exec_point) {                                                  \
        case 0:                                                                                    \
            __VA_ARGS__ *mirwa_priv_handler_exec_point = -1;                                       \
        }                                                                                          \
    }                                                                                              \
                                                                                                   \
    MIRWA_TYPEDEF_STRUCT(effect##Effect_##op##_Args)

#define resume(...)                                                                                \
    *mirwa_priv_handler_exec_point = __LINE__;                                                     \
    return __VA_ARGS__;                                                                            \
    case __LINE__:

#define handle_with(effect)                                                                        \
    mirwa_handlers = (MirwaHandlers) {                                                             \
        .prev = MIRWA_OBJ(MirwaHandlers, mirwa_handlers), .effect_discriminant = &effect##Effect,  \
        .effect_state = &(effect##EffectState){0},                                                 \
    }

#define perform_effect(effect, op, args, ...)                                                      \
    int mirwa_priv_handler_exec_point = 0;                                                         \
    while (1) {                                                                                    \
        effect##Effect##_##op(                                                                     \
            &mirwa_priv_handler_exec_point,                                                        \
            (effect##EffectState *)mirwa_find_handler(&mirwa_handlers, &effect##Effect)            \
                ->effect_state,                                                                    \
            (effect##Effect##_##op##_Args){mirwa_unit,                                             \
                                           EPILEPSY_EVAL(EPILEPSY_UNPARENTHESISE(v((v(args)))))}); \
                                                                                                   \
        if (mirwa_priv_handler_exec_point == -1) {                                                 \
            break;                                                                                 \
        }                                                                                          \
                                                                                                   \
        __VA_ARGS__                                                                                \
    }                                                                                              \
                                                                                                   \
    MIRWA_SEMICOLON_IN_BLOCK()

typedef struct MirwaHandlers {
    struct MirwaHandlers *prev;
    const void *effect_discriminant;
    void *effect_state;
} MirwaHandlers;

inline static const MirwaHandlers *mirwa_find_handler(
    const MirwaHandlers handlers[restrict MIRWA_NON_NULL],
    const void *restrict effect_discriminant) {
    while (handlers->effect_discriminant != effect_discriminant) {
        handlers = (MirwaHandlers *)handlers->prev;
    }

    return handlers;
}

#endif // MIRWA_EFFECT_H
