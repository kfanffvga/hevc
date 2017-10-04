#include "hevc_decoder/predictors/predictor_manager.h"

using std::shared_ptr;

PredictorManager::PredictorManager(ParametersManager* parameter_manager)
{

}

PredictorManager::~PredictorManager()
{

}

bool PredictorManager::Flush()
{
    return false;
}

bool PredictorManager::Decode(const shared_ptr<FrameSyntax>& frame_syntax, 
                              Frame* frame)
{
    return false;
}
