/*
 * ColonBuf.h
 *
 *  Created on: 2012-4-6
 *    
 */

#ifndef GAME_COLONBUF_H_
#define GAME_COLONBUF_H_

#include <string.h>
#include <memory.h>
class ColonBuf
{
public:
        ColonBuf(const char *buf)
        {
                m_buf = buf;
                m_len = strlen(m_buf);
                m_rindex = 0;
        }
        int GetInt(int &res)
        {
                if(m_rindex >= m_len)return 1;
                char intbuf[64];
                int i = m_rindex;
                while(i < m_len && m_buf[i] != ':')
                {
                        i++;
                }
                memcpy(intbuf, &m_buf[m_rindex], i - m_rindex);
                intbuf[i - m_rindex] = 0;
                m_rindex = i + 1;
                res = ::atoi(intbuf);
                return 0;
        }
        short GetShort()
        {
            if(m_rindex >= m_len)return 1;
            char intbuf[64];
            int i = m_rindex;
            while(i < m_len && m_buf[i] != ':')
            {
                    i++;
            }
            memcpy(intbuf, &m_buf[m_rindex], i - m_rindex);
            intbuf[i - m_rindex] = 0;
            m_rindex = i + 1;
            short res = static_cast<short>(atoi(intbuf));
            return res;
        }
        int GetIntDefault(int &res, int value)
        {
                int r = GetInt(res);
                if(r == 1)
                {
                        res = value;
                }
                return r;
        }
        int GetString(char *res)
        {
                if(m_rindex >= m_len)return 1;
                int i = m_rindex;
                while(i < m_len && m_buf[i] != ':')
                {
                        i++;
                }
                memcpy(res, &m_buf[m_rindex], i - m_rindex);
                res[i - m_rindex] = 0;
                m_rindex = i + 1;
                return 0;
        }
        int GetStringDefault(char *res, const char *def)
        {
                int r = GetString(res);
                if(r == 1)
                {
                        strcpy(res, def);
                }
                return r;
        }
        int Eof()
        {
                return (m_rindex >= m_len) ? 1 : 0;
        }
private:
        int m_rindex;
        const char *m_buf;
        int m_len;
};

#endif /* COLONBUF_H_ */
