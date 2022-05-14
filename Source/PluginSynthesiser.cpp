/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginSynthesiser.h"
#include "PluginEditor.h"

//==============================================================================
SulfuricAudioProcessor::SulfuricAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
	params(*this, nullptr, juce::Identifier("SulfuricParams"),
		{
			std::make_unique<juce::AudioParameterFloat>("master", "Master", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.1f, "", Category::outputGain)
		}
	)
#endif
{
	masterParam = params.getRawParameterValue("master");

	synth = new juce::Synthesiser();
	for (auto numVoices = 16; numVoices > 0; numVoices--) {
		synth->addVoice(new SulfuricVoice());
	}
	synth->addSound(new SulfuricSound());
}

SulfuricAudioProcessor::~SulfuricAudioProcessor()
{
}

//==============================================================================
const juce::String SulfuricAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool SulfuricAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool SulfuricAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool SulfuricAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double SulfuricAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int SulfuricAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
				// so this should be at least 1, even if you're not really implementing programs.
}

int SulfuricAudioProcessor::getCurrentProgram()
{
	return 0;
}

void SulfuricAudioProcessor::setCurrentProgram(int /*index*/)
{
}

const juce::String SulfuricAudioProcessor::getProgramName(int /*index*/)
{
	return {};
}

void SulfuricAudioProcessor::changeProgramName(int /*index*/, const juce::String& /*newName*/)
{
}

//==============================================================================
void SulfuricAudioProcessor::prepareToPlay(double sampleRate, int /*samplesPerBlock*/)
{
	// Use this method as the place to do any pre-playback
	// initialisation that you need..
	synth->setCurrentPlaybackSampleRate(sampleRate);
	prevMaster = *masterParam;
}

void SulfuricAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SulfuricAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	juce::ignoreUnused(layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
		return false;

	// This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
#endif

	return true;
#endif
}
#endif

juce::MidiBuffer SulfuricAudioProcessor::filterMidiMessagesForChannel(const juce::MidiBuffer& input, int channel)
{
	juce::MidiBuffer output;

	for (auto metadata : input)
	{
		auto message = metadata.getMessage();

		if (message.getChannel() == channel)
			output.addEvent(message, metadata.samplePosition);
	}

	return output;
}

void SulfuricAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	auto busCount = getBusCount(false);
	for (auto busNr = 0; busNr < busCount; ++busNr)
	{
		auto audioBusBuffer = getBusBuffer(buffer, false, busNr);
		auto midiChannelBuffer = filterMidiMessagesForChannel(midiMessages, busNr + 1);
		synth->renderNextBlock(audioBusBuffer, midiChannelBuffer, 0, audioBusBuffer.getNumSamples());
	}

	// Set master level last
	float currentMaster = *masterParam;
	if (currentMaster == prevMaster)
	{
		buffer.applyGain(currentMaster);
	}
	else
	{
		buffer.applyGainRamp(0, buffer.getNumSamples(), prevMaster, currentMaster);
		prevMaster = currentMaster;
	}
}

//==============================================================================
bool SulfuricAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SulfuricAudioProcessor::createEditor()
{
	return new SulfuricAudioProcessorEditor(*this, params);
}

//==============================================================================
void SulfuricAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.
	auto state = params.copyState();
	std::unique_ptr<juce::XmlElement> xml(state.createXml());
	copyXmlToBinary(*xml, destData);
}

void SulfuricAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
	std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

	if (xmlState.get() != nullptr)
		if (xmlState->hasTagName(params.state.getType()))
			params.replaceState(juce::ValueTree::fromXml(*xmlState));
}

bool SulfuricSound::appliesToNote(int /*note*/)
{
	return true;
}

bool SulfuricSound::appliesToChannel(int /*channel*/)
{
	return true;
}

bool SulfuricVoice::canPlaySound(juce::SynthesiserSound* sound)
{
	return dynamic_cast<SulfuricSound*> (sound) != nullptr;
}

void SulfuricVoice::updateAngleDelta()
{
	auto cyclesPerSample = currentFrequency / getSampleRate();
	angleDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;          // [3]
}


void SulfuricVoice::startNote(int midiNoteNumber, float velocity,
	juce::SynthesiserSound*, int /*currentPitchWheelPosition*/)
{
	currentAngle = 0.0;
	level = velocity;
	tailOff = 0.0;

	if (rampOn == 0.0) {
		rampOn = 1.0;
	}

	currentFrequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
	updateAngleDelta();
}

void SulfuricVoice::stopNote(float /*velocity*/, bool allowTailOff)
{
	rampOn = 0.0;

	if (allowTailOff)
	{
		if (tailOff == 0.0)
			tailOff = 1.0;
	}
	else
	{
		clearCurrentNote();
		angleDelta = 0.0;
	}
}

void SulfuricVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
	if (angleDelta != 0.0)
	{
		if (tailOff > 0.0)
		{
			while (--numSamples >= 0)
			{
				auto currentSample = (float)(std::sin(currentAngle) * level * tailOff);

				for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
					outputBuffer.addSample(i, startSample, currentSample);

				currentAngle += angleDelta;
				++startSample;

				tailOff *= 0.99;

				if (tailOff <= 0.005)
				{
					clearCurrentNote();

					angleDelta = 0.0;
					break;
				}
			}
		}
		else if (rampOn > 0.0) 
		{
			while (--numSamples >= 0)
			{
				auto currentSample = (float)(std::sin(currentAngle) * level * (1 - rampOn));

				for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
					outputBuffer.addSample(i, startSample, currentSample);

				currentAngle += angleDelta;
				++startSample;

				rampOn *= 0.99;

				if (rampOn <= 0.005)
				{
					rampOn = 0.0;
				}
			}
		}
		else
		{
			while (--numSamples >= 0)
			{
				auto currentSample = (float)(std::sin(currentAngle) * level);

				for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
					outputBuffer.addSample(i, startSample, currentSample);

				currentAngle += angleDelta;
				++startSample;
			}
		}
	}
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new SulfuricAudioProcessor();
}