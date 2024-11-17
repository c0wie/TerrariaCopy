#pragma once
#include <array>

/*
    octaves - increas complexity of map
    bias - controls smoothness of terrain. Higher bias smoother terrain, lower bias rougher terrain  
*/
template<unsigned int Count>
std::array<float, Count> PerlinNoise1D(const std::array<float, Count> &seed, int octaves, float bias)
{
    std::array<float, Count> perlinNoise;
    for(int i = 0; i < perlinNoise.size(); i++)
    {
        float noise = 0.0f;
        float scaleAcc= 0.0f;
        float scale = 1.0f;

        for(int o = 0; o < octaves; o++)
        {
            // bitwise right shift effectively divides expression by power of 2
            int pitch = Count >> o;
            // int truncation so (i / pitch) * pitch won't give same result as just i
            int sample1 = (i / pitch) * pitch;
            // wrap around at array end by using '%'
            int sample2 = (sample1 + pitch) % Count;
            float blend = (float)(i - sample1) / (float)pitch;
            float sample = (1.0f - blend) * seed[sample1] + blend * seed[sample2];

            scaleAcc += scale;
            noise += sample * scale;
            scale /= bias;
        }
        perlinNoise[i] = noise / scaleAcc;
    }
    return perlinNoise;
}

template <unsigned int Width, unsigned int Height> 
std::vector<float> PerlinNoise2D(const std::vector<float> &seed, int octaves, float bias)
{
    std::vector<float> perlinNoise;
    perlinNoise.resize(Width * Height);
    if(seed.size() != perlinNoise.size())
    {
        std::cerr << "seed size doesn't match noise size\n";
        std::exit(1);
    }
    for(int x = 0; x < Width; x++)
    {
        for(int y = 0; y < Height; y++)
        {
            float noise = 0.0f;
            float scaleAcc= 0.0f;
            float scale = 1.0f;

            for(int o = 0; o < octaves; o++)
            {
                int pitch = Width >> o;
				int nSampleX1 = (x / pitch) * pitch;
				int nSampleY1 = (y / pitch) * pitch;
				
				int nSampleX2 = (nSampleX1 + pitch) % Width;					
				int nSampleY2 = (nSampleY1 + pitch) % Width;

				float fBlendX = (float)(x - nSampleX1) / (float)pitch;
				float fBlendY = (float)(y - nSampleY1) / (float)pitch;

				float fSampleT = (1.0f - fBlendX) * seed[nSampleY1 * Width + nSampleX1] + fBlendX * seed[nSampleY1 * Width + nSampleX2];
				float fSampleB = (1.0f - fBlendX) * seed[nSampleY2 * Width + nSampleX1] + fBlendX * seed[nSampleY2 * Width + nSampleX2];

		        scaleAcc += scale;
				noise += (fBlendY * (fSampleB - fSampleT) + fSampleT) * scale;
				scale = scale / bias;
            }
            perlinNoise[y * Width + x] = noise / scaleAcc;
        }
    }
    return perlinNoise;
}