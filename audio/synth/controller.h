// controller.h
// Author: Allen Porter <allen@thebends.org>
//
// The controller module returns samples and drives the oscillator and envelope.

namespace synth {

class Envelope;
class LFO;
class Oscillator;

class Controller {
 public:
  Controller();

  int sample_rate();
  void set_sample_rate(int sample_rate);

  // Volume [0.0, 1.0]
  float volume();
  void set_volume(float volume);

  // The envelope and oscillator must be set before generating samples.
  void set_oscillator(Oscillator* oscillator);
  void set_volume_envelope(Envelope* envelope);
  // TODO(allen): Filter envelope
  void set_lfo(LFO* lfo);

  void GetSamples(int num_output_samples, float* output_buffer);

 private:
  int sample_rate_;
  int sample_;
  float volume_;
  Oscillator* oscillator_;
  Envelope* volume_envelope_;
  LFO* lfo_;
};

}  // namespace synth
