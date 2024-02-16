# Twang

![Twang cover](./.github/cover.png)

A simple and aesthetic digital tuner plugin.

## Installation

> [!WARNING]
> I have not tried following these instructions myself and have no idea if they will work on another machine. I also only know that it builds at all using Make on MacOS, and only verified the AU plugin works properly.

1. Clone this repository and its submodules

   ```
   git clone --recursive https://github.com/Jklein64/twang.git
   cd twang
   ```

2. Build all the targets (or you can pick I guess)

   ```
   cmake --build ./build --target all
   ```

3. Open a DAW and look for the plugin!

## How it works

Whenever a block of audio gets routed into the plugin, `PluginProcessor::processBlock` puts it at the end of a circular buffer. When the buffer is full, it takes its Fourier Transform and then applies the Harmonic Product Spectrum (HPS) algorithm (simple explanation of that [here](http://musicweb.ucsd.edu/~trsmyth/analysis/Harmonic_Product_Spectrum.html)) to identify the most dominant pitch. From there, `Notes::freq_to_note` finds the closest Equal Temperament note and its pitch, which is used to guage how well tuned the original audio is. Then the oldest couple blocks of samples are removed from the buffer and the process repeats.

Since I intended to (try to) use this for bass tuning as well as guitar tuning, I needed it to be able to differentiate the lowest note a bass can play and one semitone lower, so E1 and Eb1. This means that, for sample rate $f_s$ and pitches $p_{E_1}$ and $p_{Eb_1}$ in Hz, the size of my FFT buffer needs to be the smallest power of two greater than $f_s / (p_{E_1} - p_{Eb_1})$. This is implemented [here](https://github.com/Jklein64/twang/blob/4bc8804324b02251e3fedac2f59e400393ba39b3/source/PluginProcessor.cpp#L95), and led to huge FFT sizes...

## If I did it all again...

- I'd have tested my algorithm in advance of the implementation. While HPS works pretty well for detecting the note, it has poor resolution, so the data about how close the observed pitch is to the closest note of the (12 TET) [Equal Temperament](https://en.wikipedia.org/wiki/Equal_temperament) scale is _extremely_ noisy. This made it difficult to debug my accuracy bars UI. Future work might involve median filtering or a second pass with a battery of band-pass filters to more accurately measure the deviation from Equal Temperament tuning.
- I would figure out how to render an SVG or at least parse the relevant information from the design and use it in `PluginEditor::paint`. Manually entering the path control point coordinates from Affinity Designer is definitely not scalable beyond the couple of points that I had, and it still took me a whole tedious evening to transfer them over
- I'd avoid using [FFTW](https://www.fftw.org/) and opt for a simpler FFT library with a more idiomatic C++ interface. FFTW's promise of speed was really appealing given the size of the Fourier Transforms I ended up taking ($N = 2^{15} = 32768$ samples), but the C-like API forced me to either use `float*` instead of `std::vector<float>` (I like vectors...) or do some [trickery](https://github.com/Jklein64/twang/blob/4bc8804324b02251e3fedac2f59e400393ba39b3/source/PluginProcessor.cpp#L103) with `reinterpret_cast<>` (to interchange between `fftwf_complex` and `std::complex<float>`, since I couldn't make a `std::vector` of the former), and also had to resort to copying when "implementing" a [circular buffer](https://github.com/Jklein64/twang/blob/4bc8804324b02251e3fedac2f59e400393ba39b3/source/PluginProcessor.cpp#L254) since FFTW's transform functions don't take iterators
- I also wouldn't use [PampleJUCE](https://github.com/sudara/pamplejuce). It was nice having a template, but it came with a lot of features that weren't applicable to me (like the MacOS code signing GitHub Actions workflow which requires a full Apple Developer account...). After learning the basics from its [resources on CMake](https://github.com/sudara/pamplejuce?tab=readme-ov-file#cmake), I think I'll just configure the whole project myself next time
