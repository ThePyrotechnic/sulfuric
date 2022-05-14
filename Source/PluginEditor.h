/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "PluginSynthesiser.h"

//==============================================================================
/**
*/
class SulfuricAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
	SulfuricAudioProcessorEditor(SulfuricAudioProcessor&, juce::AudioProcessorValueTreeState&);
	~SulfuricAudioProcessorEditor() override;

	typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

	void paint(juce::Graphics&) override;
	void resized() override;

	const juce::Colour metalGrey{ 0xFF3B3B3B };
	const juce::Colour brightMetalGrey = metalGrey.brighter(0.3f);

	const juce::Colour offYellow{ 0xFFAF9F5B };
	const juce::Colour offWhite{ 0xFFEDEDED };

	const static int WIDTH = 1280;
	const static int HEIGHT = 720;

	const static int SMALL_SPACE = WIDTH / 20;

	const static int SMALL_TEXT_W = 50;
	const static int SMALL_TEXT_H = 20;

	const static int MEDIUM_TEXT_W = 100;
	const static int MEDIUM_TEXT_H = 30;

	const static int LARGE_TEXT_W = 200;
	const static int LARGE_TEXT_H = 60;

	const static int SMALL_ROTARY_W = SMALL_SPACE;
	const static int SMALL_ROTARY_H = SMALL_SPACE + SMALL_TEXT_H;

	const static int SMALL_BUTTON = WIDTH / 50;

	const static int LARGE_BUTTON = WIDTH / 5;

	const static int KNOBS_PER_ROW = 9;
	const static int ROWS = 2;
	const static size_t KNOB_COUNT = KNOBS_PER_ROW * ROWS;
	const static size_t KNOB_SPACING = (WIDTH - (KNOBS_PER_ROW * SMALL_ROTARY_W)) / (KNOBS_PER_ROW + 1);


private:
	void configureRotary(juce::Slider&);

	void configureButton(juce::ShapeButton&, bool);

	uint64_t randomizeSeed();

	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	SulfuricAudioProcessor& audioProcessor;

	juce::AudioProcessorValueTreeState& valueTreeState;

	juce::Random rng;

	juce::Slider masterSlider;
	std::unique_ptr<SliderAttachment> masterAttachment;

	juce::ImageButton resetButton;

	juce::Label monoButtonLabel{ "mono-btn-lbl", "MONO" };
	juce::ShapeButton monoButton{ "mono-btn", brightMetalGrey, brightMetalGrey, brightMetalGrey };

	juce::Label presetButtonLabel{ "preset-btn-lbl", "PRE" };
	juce::ShapeButton presetButton{ "preset-btn", brightMetalGrey, brightMetalGrey, brightMetalGrey };

	juce::Label saveButtonLabel{ "save-btn-lbl", "SAVE" };
	juce::ShapeButton saveButton{ "save-btn", brightMetalGrey, brightMetalGrey, brightMetalGrey };

	juce::Label seedLabel;

	std::array<juce::Slider, KNOB_COUNT> parameterKnobs;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SulfuricAudioProcessorEditor)
};
