#ifndef _CHROMA_QP_OFFSET_H_
#define _CHROMA_QP_OFFSET_H_

class CABACReader;
class IChromaQPOffsetContext;

class ChromaQPOffset
{
public:
    ChromaQPOffset();
    ~ChromaQPOffset();

    bool Parse(CABACReader* cabac_reader, IChromaQPOffsetContext* context);

private:

};

#endif