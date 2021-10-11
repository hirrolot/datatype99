#include <datatype99.h>

#include <stdio.h>

// Deriver implementation {

#define DATATYPE99_DERIVE_Menu_IMPL(name, variants)                                                \
    ML99_prefixedBlock(                                                                            \
        v(inline static void name##_print_menu(void)),                                             \
        ML99_listMapInPlace(ML99_compose(v(genCommand), v(ML99_untuple)), v(variants)))

#define genCommand_IMPL(tag, _sig)                                                                 \
    ML99_TERMS(                                                                                    \
        DATATYPE99_assertAttrIsPresent(v(tag##_Menu_description)),                                 \
        v(ML99_IF(DATATYPE99_ATTR_IS_PRESENT(tag##_Menu_note), GEN_NOTE, ML99_EMPTY)(tag)),        \
        ML99_invokeStmt(                                                                           \
            v(printf),                                                                             \
            v(#tag ": %s.\n"),                                                                     \
            DATATYPE99_attrValue(v(tag##_Menu_description))))
#define genCommand_ARITY 1

#define GEN_NOTE(tag) printf("(" DATATYPE99_ATTR_VALUE(tag##_Menu_note) ") ");
// } (Deriver implementation)

typedef const char *MessageContent;
typedef int UserId;
typedef int ChannelId;

#define SendMessage_Menu_description        attr("Send a private message to someone")
#define SubscribeToChannel_Menu_description attr("Subscribe to channel")
#define DeleteAccount_Menu_description      attr("Delete my account")
#define DeleteAccount_Menu_note             attr("DANGEROUS")

// clang-format off
datatype(
    derive(Menu),
    UserCommand,
    (SendMessage, MessageContent, UserId),
    (SubscribeToChannel, ChannelId),
    (DeleteAccount)
);
// clang-format on

#undef SendMessage_Menu_description
#undef SubscribeToChannel_Menu_description
#undef DeleteAccount_Menu_description
#undef DeleteAccount_Menu_note

/*
 * Output:
 *
 * SendMessage: Send a private message to someone.
 * SubscribeToChannel: Subscribe to channel.
 * (DANGEROUS) DeleteAccount: Delete my account.
 */
int main(void) {
    UserCommand_print_menu();

    return 0;
}
