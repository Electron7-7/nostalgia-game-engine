#ifndef GLM_FUNCTIONS_H
#define GLM_FUNCTIONS_H

template<GLMContainer T> consteval int GLMContainerSize() { return -1; }
template<> consteval int GLMContainerSize<glm::vec2>() { return 2; }
template<> consteval int GLMContainerSize<glm::vec3>() { return 3; }
template<> consteval int GLMContainerSize<glm::vec4>() { return 4; }
template<> consteval int GLMContainerSize<glm::quat>() { return 4; }

template<GLMContainer T, int Size = GLMContainerSize<T>()>
    void FillVectorWithChangedValue(Farg<T> inOldVector, T& outNewVector)
    {
        if constexpr(Size == -1)
            { return; }
        for(int i{0}; i < Size; ++i)
        {
            float _val{outNewVector[i]};
            if(_val != inOldVector[i])
            {
                outNewVector = T{_val};
                return;
            }
        }
    }

#endif // GLM_FUNCTIONS_H
