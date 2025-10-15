#ifndef CONVERSION_H
#define CONVERSION_H

#pragma message("FIXME: These aren't great")
template<typename J, typename G>
    G JoltToGlm(const J& Jolt);
template<typename J, typename G>
    void JoltToGlm(const J& Jolt, G& Glm);

template<typename J, typename G>
    J GlmToJolt(const G& Glm);
template<typename J, typename G>
    void GlmToJolt(const G& Glm, J& Jolt);

#endif // CONVERSION_H
