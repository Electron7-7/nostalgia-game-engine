#ifndef CONVERSION_H
#define CONVERSION_H

#pragma message("FIXME: These aren't great")
template<typename G, typename J>
    G JoltToGlm(const J& Jolt);
template<typename G, typename J>
    void JoltToGlm(const J& Jolt, G& Glm);

template<typename J, typename G>
    J GlmToJolt(const G& Glm);
template<typename J, typename G>
    void GlmToJolt(const G& Glm, J& Jolt);

#endif // CONVERSION_H
