// controller.h
// Author: Allen Porter <allen@thebends.org>

namespace oscillators { class Oscillator; }
namespace envelope { class Envelope; }

namespace controller {

class Controller {
 public:
  Controller(int sample_rate);

  float volume();
  void set_volume(float volume);

  void set_oscillator(oscillators::Oscillator* oscillator);
  void set_volume_envelope(envelope::Envelope* envelope);

  void GetSamples(int num_output_samples, float* output_buffer);

 private:
  int sample_rate_;
  int sample_;
  float volume_;
  oscillators::Oscillator* oscillator_;
  envelope::Envelope* volume_envelope_;
};

}  // namespace controller
