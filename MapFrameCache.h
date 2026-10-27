#pragma once
#include <array>
// Cached composition checkpoints preserve GDI/alpha drawing exactly. A change
// in a lower group invalidates all later checkpoints, but never earlier ones.
#pragma pack(push,8)
class CMapFrameCache 
{
    struct Surface 
    {
        CDC dc; 
        CBitmap bitmap; 
        HGDIOBJ original=nullptr; 
        bool valid=false;
        void Clear()
        {
            if(dc.GetSafeHdc()&&original)
                ::SelectObject(dc.GetSafeHdc(),original);
            original=nullptr;
            bitmap.DeleteObject();
            dc.DeleteDC();
            valid=false;
        }
        ~Surface()
        {
            Clear();
        }
    } surfaces[4];

    int width=0,height=0;std::array<double,12> key={};
    bool keyed=false;

public:
    enum Group{Elevation=0,Background=1,Sources=2,Shapes=3};
    unsigned long long renders[4]={},hits[4]={};
    void InvalidateFrom(int first=0)
    {
        for(int i=first;i<4;++i)
            surfaces[i].valid=false;
    }
    void Prepare(int w,int h,const std::array<double,12>& next)
    {
        if(w!=width||h!=height)
        {
            for(auto&s:surfaces)
                s.Clear();
            width=w;
            height=h;
        }
        if(!keyed||key!=next)
        {
            InvalidateFrom();
            key=next;
            keyed=true;
        }
    }
    bool Restore(int group,CDC* target)
    {
        auto&s=surfaces[group];
        if(!s.valid)
            return false;
        if(!target->BitBlt(0,0,width,height,&s.dc,0,0,SRCCOPY))
        {
            InvalidateFrom(group);
            return false;
        }
        ++hits[group];
        
        return true;
    }
    void Store(int group,CDC* source)
    {
        auto&s=surfaces[group];
        ++renders[group];
        if(width<=0||height<=0)
            return;
        if(!s.dc.GetSafeHdc())
        {
            if(!s.dc.CreateCompatibleDC(source)||!s.bitmap.CreateCompatibleBitmap(source,width,height))
            {
                s.Clear();
                return;
            }
            s.original=::SelectObject(s.dc.GetSafeHdc(),s.bitmap.GetSafeHandle());
            if(!s.original||s.original==HGDI_ERROR)
            {
                s.original=nullptr;
                s.Clear();
                return;
            }
        }
        s.valid=s.dc.BitBlt(0,0,width,height,source,0,0,SRCCOPY)!=FALSE;
    }
};
#pragma pack(pop)
