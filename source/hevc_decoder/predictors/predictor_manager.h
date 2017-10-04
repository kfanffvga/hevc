#ifndef _PREDICTOR_MANAGER_H_
#define _PREDICTOR_MANAGER_H_

#include <memory>

class Frame;
class FrameSyntax;
class ParametersManager;

class PredictorManager
{
public:
    PredictorManager(ParametersManager* parameter_manager);
    ~PredictorManager();

    bool Flush();
    bool Decode(const std::shared_ptr<FrameSyntax>& frame_syntax, Frame* frame);

private:
    ParametersManager* parameter_manager_;
};

#endif