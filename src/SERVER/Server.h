#ifndef Server_First
#define Server_First

typedef enum ServerMessageTags {
    eMsgNone,
    
    eMsgMakeWindow,
    eMsgMakeBitmap,
    eMsgMakeFont,
    
    eMsgDestroy,
    eMsgClip,
    eMsgResetClip,
    eMsgStrokeLine,
    eMsgStrokeRect,
    eMsgFillRect,
    eMsgStrokeOval,
    eMsgFillOval,
    eMsgStrokeRRect,
    eMsgFillRRect,
    eMsgStrokeWedge,
    eMsgFillWedge,
    eMsgStrokePolygon,
    eMsgFillPolygon,
    eMsgShowBitmap,
    eMsgShowTextBatch,
    eMsgGiveHint,
    
    eMsgScrollRect,
    eMsgTop,
    eMsgHide,
    eMsgShow,
    eMsgSetRect,
    eMsgGrab,
    eMsgSetMousePos,
    eMsgBell,
    eMsgSetCursor,

    eMsgShutDown,
    eMsgDelay,
    
    eMsgHaveClipboard,
    eMsgGetClipboard,
    eMsgSendClipboard,
    eMsgClipboardEmpty,
    eMsgNotOwner

} ServerMessages;

struct Message {
    short tag, seq;
    short id, s1, s2, s3;
    int i1;
    Point p1, p2;
    Rectangle r1;
};

struct Response {
    WindowPort *port;
    Token t;
};

#define SERVERPATH "/tmp"
#define SERVERNAME "ET++"
#define SERVICENAME "et++"
#define PROTOCOLNAME "tcp"


#endif Server_First
