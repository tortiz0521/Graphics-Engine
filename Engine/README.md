### Some interesting things to note!
While going through the process of making this small-scale engine, I asked myself a question about texture configs. Obviously, i we never plan on changing the configuration of a specific tecture, and therefore never attempting to generate another texture, this wouldn't necessarily be a problem; but what if we DID want to change what a texture is after it generates? There are a few questions I asked myself:

    - Do we store each config variable in each class?
    - How do we go about reasonably changing the configs without needing to make unnecessary calls to setters?
    - Do I REALLY want all of my configs publicly avaialable and able to be changed at any given moment?

My short answer to each of these questions is no/you SHOULDN'T!

There are 8 different config vars that we could potentially set in a single class!!! This means at least 24 bytes of extra memory PER TEXTURE CLASS. While it would be very convenient to do this, the issue becomes quite apparent when there are 100s or even 1000s of textures in a game to generate!

Here was my thought: What if there was some way to store configs in a map of some sort that can then be easily accessed at any time? The MOST that we would have to do is store some weak_ptr to that config, but how do we organize configs? How do we access configs? So I went and did a little bit of research, here's what I found:

To store a config is simple: put it into a struct! Structs have no overhead, and effectively act as a way to contiguously store different pieces of data together! A neat aspect of structs that I always find interesting is how we can apply functions to our structs, making some small C-like objects. This is where we can really create an optimization; when we want to make a new texture, we simply need to create some struct with all of the configs, then compare that to a structs within a map! If the that specific config setup already exists, then we don't need to create a new one. If not, we store the config in our map for later use!

How does this help us? In the case that we want to potentially use multiple configs on a single texture, this reduces the size of our texture class, needing only 8 bytes to store a pointer instead of 24+ for all of the configs! So how exactly do we map these configs to some data structure? This is where I learned a little bit more about hashing functions themselves, and the context with which we can use them! 

We start with a simple hash of an integer:

    size_t h = std::hash<int>()(c._wrapS);

This is just a hash of an integer. Now we want to include all of our other hashes as well:

    h ^= std::hash<int>()(c._wrapT) << 1;
    h ^= std::hash<int>()(c._filterMin) << 2;
    h ^= std::hash<int>()(c._filterMax) << 3;
    h ^= std::hash<bool>()(c._generateMipmaps);

We XOR the results from each hashing function with the current 'h' after we also shifting the bits. A bit shift is computed to simply ensure that different similar inputs from the variables are more difficult to recreate when they are in differing orders, thus lessening the amount of collisions. An XOR calculation is also performed as a way to "increase entropy" or effectively mix up the bits more. The final hash with the bool is not bit shifted since a bool is only 1 bit. While it could be nice to shift this bit, we already get a copmlete bit flipping from it anyways.


This was the code that would contribute to a texture config setup! The 

    // The general struct for a texture config.
    struct TextureConfig
    {
        // Texture format:
        unsigned int _internal, _image;

        // Texture config:
        unsigned int _wrapS, _wrapT; // Wrapping modes on s-axis/t-axis.
        unsigned int _filterMin, _filterMax; // Filtering modes for if texels < pixels / texels > pixels
        bool _generateMipmaps;

        bool operator==(const TextureConfig& o) const 
        {
            return (_internal == o._internal && _image == o._image && 
                _wrapS == o._wrapS && _wrapT == o._wrapT &&
                _filterMin == o._filterMin && _filterMax == o._filterMax);
        }
    };

    // The hashing function. This is held in a seperate struct so that we can pass it to the
    // std::unordered_map later and not incur any overhead as a lambda would.
    struct TextureConfigHash
    {
        std::size_t operator()(const TextureConfig &c) const
        {
            std::size_t h = std::hash<int>()(c._wrapS);
            h ^= std::hash<int>()(c._wrapT) << 1;
            h ^= std::hash<int>()(c._filterMin) << 2;
            h ^= std::hash<int>()(c._filterMax) << 3;
            h ^= std::hash<bool>()(c._generateMipmaps);
            return h;
        }
    };
