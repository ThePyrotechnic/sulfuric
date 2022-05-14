/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

//==============================================================================
/**
*/
class SulfuricAudioProcessor : public juce::AudioProcessor
{
public:
	//==============================================================================
	SulfuricAudioProcessor();
	~SulfuricAudioProcessor() override;

	typedef juce::AudioProcessorParameter::Category Category;

	//==============================================================================
	void prepareToPlay(double, int) override;
	void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported(const BusesLayout&) const override;
#endif

	void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

	//==============================================================================
	juce::AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override;

	//==============================================================================
	const juce::String getName() const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool isMidiEffect() const override;
	double getTailLengthSeconds() const override;

	//==============================================================================
	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram(int) override;
	const juce::String getProgramName(int) override;
	void changeProgramName(int, const juce::String&) override;

	//==============================================================================
	void getStateInformation(juce::MemoryBlock&) override;
	void setStateInformation(const void*, int) override;

	//==============================================================================
	std::atomic<float>* masterParam;
	float prevMaster;

private:
	juce::Synthesiser* synth;

	juce::AudioProcessorValueTreeState params;
	//==============================================================================
	static juce::MidiBuffer filterMidiMessagesForChannel(const juce::MidiBuffer&, int);

	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SulfuricAudioProcessor)
};

//==============================================================================
struct SulfuricSound : public juce::SynthesiserSound
{
    SulfuricSound() {}

	bool appliesToNote(int) override;
	bool appliesToChannel(int) override;
};

//==============================================================================
struct SulfuricVoice : public juce::SynthesiserVoice
{
    SulfuricVoice() {}

    bool canPlaySound(juce::SynthesiserSound*) override;
	void startNote(int, float, juce::SynthesiserSound*, int) override;
	void stopNote(float, bool) override;
    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}
	void renderNextBlock(juce::AudioBuffer<float>&, int, int) override;

private:
	void updateAngleDelta();
    double currentAngle = 0.0, angleDelta = 0.0, level = 0.0, tailOff = 0.0, rampOn = 0.0;
	double currentFrequency = 0.0, targetFrequency = 0.0;
};