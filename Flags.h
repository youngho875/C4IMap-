
#ifndef _FLAGS_H
#define _FLAGS_H


// 비트플래그를 설정하는 기초클래스  06.07.21 : 유혁재
class CFlags {
public:
    // 기본생성자. 모든 플래그를 0으로 초기화한다.
    CFlags() {
        flags = 0;
    }

    // 주어진 플래그 값으로 초기화한다.
    CFlags(unsigned int f) {
        flags = f;
    }

    virtual ~CFlags() {}

    unsigned int getFlags() const {
        return flags;
    }

    void resetFlags() {
        flags=0;
    }

    void setFlags(unsigned int f) {
        flags=f;
    }

    void setFlag(unsigned int f) {
        flags=flags|f;
    }

    void delFlag(unsigned int f) {
        flags=flags&(~f);
    }

    void toggleFlag(unsigned int f) {
        flags=flags^f;
    }

    BOOL getFlag(unsigned int f) const {
		if(flags&f) {
            return TRUE;
        } else {
            return FALSE;
        }
    }


private:
    unsigned int flags;
};

#endif
