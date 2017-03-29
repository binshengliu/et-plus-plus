#ifndef Clipboard_First
#define Clipboard_First

#define SERVERNAME "ET++clipboard"
#define SERVICENAME "et++clipboard"

typedef enum ServerMessageTags {
    eMsgNone,
    eMsgError,
    eMsgHaveClipboard,
    eMsgGetClipboard,
    eMsgSendClipboard,
    eMsgClipboardEmpty,
    eMsgNotOwner,
    eMsgCloseDown,
    eMsgEnroll
} ServerMessages;

struct Header {
    ServerMessages tag;
    u_int len;
};

#endif Clipboard_First

