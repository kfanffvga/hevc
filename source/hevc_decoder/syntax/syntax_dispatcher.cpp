#include "hevc_decoder/syntax/syntax_dispatcher.h"
#include "hevc_decoder/syntax/nal_unit.h"
#include "hevc_decoder/syntax/video_parameter_set.h"

using std::unique_ptr;
using std::move;

SyntaxDispatcher::SyntaxDispatcher()
{

}

SyntaxDispatcher::~SyntaxDispatcher()
{

}

bool SyntaxDispatcher::CreateSyntaxAndDispatch(unique_ptr<NalUnit> nal_unit)
{
    if (!nal_unit)
        return false;

    switch (nal_unit->GetNalUnitType())
    {
        case NalUnitType::VPS_NUT:
        {
            unique_ptr<VideoParameterSet> vps(new VideoParameterSet());
            return vps->Parse(nal_unit->GetBitSteam());
        }
        default:
            break;
    }

    return true;
}