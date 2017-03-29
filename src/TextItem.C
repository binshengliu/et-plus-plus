//$TextItem$

#include "TextItem.h"
#include "String.h"

//---- TextItem ----------------------------------------------------------------

Point gBorder(4, 3);

MetaImpl(TextItem, (TP(text), TP(font), T(border), 0));

TextItem::TextItem(char *t, FontPtr f, Point b)
{
    font= f;
    border= b;
    text= 0;
    strreplace(&text, t);
    SetFlag(eVObjVFixed);
}

TextItem::TextItem(int id, char *t, FontPtr f, Point b) : VObject(id)
{
    font= f;
    border= b;
    text= 0;
    strreplace(&text, t);
    SetFlag(eVObjVFixed);
}

TextItem::~TextItem()
{
    SafeDelete(text);
}

void TextItem::SetString(char *t, bool redraw)
{
    strreplace(&text, t);
    SetContentRect(Rectangle(GetOrigin(), GetMinSize().extent), redraw);
}

void TextItem::SetFString(bool redraw, char *va_(fmt), ...)
{
    va_list ap;
    va_start(ap,va_(fmt));
    if (redraw) {
	InvalidateRect(contentRect);
	strfreplace(&text, va_(fmt), ap);
	CalcExtent();
	InvalidateRect(contentRect);      
    } else
	strfreplace(&text, va_(fmt), ap);
    Changed();
    va_end(ap);
}

void TextItem::SetFont(FontPtr fp)
{
    font= fp;
}

Metric TextItem::GetMinSize()
{
    Metric m= Metric(font->Width((byte*)text), font->Spacing(), font->Ascender()).Expand(border);
    return m;
}

void TextItem::SetExtent(Point)
{
    VObject::SetExtent(GetMinSize().extent);
}

int TextItem::Base()
{
    return font->Ascender() + border.y;
}

char *TextItem::AsString()
{
    return text;
}

void TextItem::Draw(Rectangle)
{
    if (text && *text) {
	Point p= contentRect.origin+border;
	p.y+= font->Ascender();
	GrShowString(font, Enabled() ? ePatBlack : ePatGrey50, p, (byte*)text);
    }
}

ostream& TextItem::PrintOn (ostream &s)
{
    VObject::PrintOn(s);
    s << font SP << border SP;
    return PrintString(s, (byte*)text);
}

istream& TextItem::ReadFrom(istream &s)
{
    SafeDelete(text);
    VObject::ReadFrom(s);
    s >> font >> border;
    return ReadString(s, (byte**) &text);
}

void TextItem::InspectorId(char *buf, int bufSize)
{
    strn0cpy(buf, text, bufSize);
}

