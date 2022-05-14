/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginSynthesiser.h"
#include "PluginEditor.h"
#include "BinaryData.h"

//==============================================================================
SulfuricAudioProcessorEditor::SulfuricAudioProcessorEditor(SulfuricAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
	: AudioProcessorEditor(&p), audioProcessor(p), valueTreeState(vts)
{
	// Make sure that before the constructor has finished, you've set the
	// editor's size to whatever you need it to be.
	setSize(WIDTH, HEIGHT);
	setResizable(true, false);

	masterAttachment.reset(new SliderAttachment(valueTreeState, "master", masterSlider));
	addAndMakeVisible(masterSlider);
	configureRotary(masterSlider);

	for (auto &knob : parameterKnobs)
	{
		addAndMakeVisible(knob);
		configureRotary(knob);
	}

	addAndMakeVisible(resetButton);
	juce::Image resetBtnImage = juce::ImageFileFormat::loadFrom(BinaryData::resetbtn_png, (size_t)BinaryData::resetbtn_pngSize);
	juce::Image resetBtnPressedImage = juce::ImageFileFormat::loadFrom(BinaryData::resetbtnpressed_png, (size_t)BinaryData::resetbtnpressed_pngSize);
	resetButton.setImages(
		false, true, true,
		resetBtnImage, 1, juce::Colours::transparentBlack,
		resetBtnImage, 1, juce::Colours::transparentBlack,
		resetBtnPressedImage, 1, juce::Colours::transparentBlack
	);

	addAndMakeVisible(monoButton);
	addAndMakeVisible(monoButtonLabel);
	configureButton(monoButton, true);

	addAndMakeVisible(presetButton);
	addAndMakeVisible(presetButtonLabel);
	configureButton(presetButton, true);

	addAndMakeVisible(saveButton);
	addAndMakeVisible(saveButtonLabel);
	configureButton(saveButton, false);

	addAndMakeVisible(seedLabel);

	std::ostringstream hexBuffer;
	hexBuffer << std::hex << randomizeSeed();
	seedLabel.setText(juce::String(hexBuffer.str()), juce::NotificationType::sendNotification);
}

SulfuricAudioProcessorEditor::~SulfuricAudioProcessorEditor() {}

//==============================================================================
void SulfuricAudioProcessorEditor::paint(juce::Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(metalGrey);
}

void SulfuricAudioProcessorEditor::resized()
{
	masterSlider.setBounds(WIDTH - SMALL_SPACE * 2, SMALL_SPACE + SMALL_BUTTON, SMALL_ROTARY_W, SMALL_ROTARY_H);

	// 3.5 should be 4 (Move the button 1 quarter to the right to center it), but the rotary knob does not visually fit into its bounding box, so 3.5 works better
	monoButton.setBounds(WIDTH - SMALL_SPACE * 2 + SMALL_ROTARY_W / 3.5, SMALL_SPACE, SMALL_BUTTON, SMALL_BUTTON);
	monoButtonLabel.setBounds(WIDTH - SMALL_SPACE * 2 + SMALL_ROTARY_W / 10, SMALL_SPACE - SMALL_BUTTON, SMALL_TEXT_W, SMALL_TEXT_H);

	resetButton.setBounds(WIDTH / 2 + WIDTH / 4 - LARGE_BUTTON / 2, HEIGHT / 4 - LARGE_BUTTON / 2, LARGE_BUTTON, LARGE_BUTTON);

	presetButton.setBounds(KNOB_SPACING, HEIGHT / 2 - SMALL_ROTARY_H, SMALL_BUTTON, SMALL_BUTTON);
	presetButtonLabel.setBounds(KNOB_SPACING, HEIGHT / 2 - SMALL_ROTARY_H - SMALL_BUTTON, SMALL_TEXT_W, SMALL_TEXT_H);

	saveButton.setBounds(KNOB_SPACING + KNOB_SPACING / 2, HEIGHT / 2 - SMALL_ROTARY_H, SMALL_BUTTON, SMALL_BUTTON);
	saveButtonLabel.setBounds(KNOB_SPACING + KNOB_SPACING / 2, HEIGHT / 2 - SMALL_ROTARY_H - SMALL_BUTTON, SMALL_TEXT_W, SMALL_TEXT_H);

	seedLabel.setBounds(SMALL_SPACE, 0, LARGE_TEXT_W, LARGE_TEXT_H);

	int currentRow = -1;
	for (size_t a = 0; a < KNOB_COUNT; a++)
	{
		if (a % KNOBS_PER_ROW == 0)
			currentRow += 1;

		parameterKnobs[a].setBounds(KNOB_SPACING + (KNOB_SPACING + SMALL_ROTARY_W) * (a % KNOBS_PER_ROW), HEIGHT / 2 + SMALL_ROTARY_H * currentRow, SMALL_ROTARY_W, SMALL_ROTARY_H);
	}

	//DBG(
	//	"WIDTH: " << WIDTH << " HEIGHT: " << HEIGHT << "\n"
	//	<< " SMALL_SPACE: " << SMALL_SPACE << "\n"
	//	<< " SMALL_TEXT_W: " << SMALL_TEXT_W << " SMALL_TEXT_H: " << SMALL_TEXT_H << "\n"
	//	<< " SMALL_ROTARY_W: " << SMALL_ROTARY_W << " SMALL_ROTARY_H: " << SMALL_ROTARY_H << "\n"
	//	<< "KNOB_SPACING: " << KNOB_SPACING
	//);
}

void SulfuricAudioProcessorEditor::configureRotary(juce::Slider &slider)
{
	slider.setSliderStyle(juce::Slider::Rotary);
	slider.setRotaryParameters(0, juce::MathConstants<float>::twoPi, true);
	slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, SMALL_TEXT_W, SMALL_TEXT_H);
	slider.setColour(juce::Slider::textBoxOutlineColourId, metalGrey);
	slider.setColour(juce::Slider::rotarySliderOutlineColourId, offWhite);
	slider.setColour(juce::Slider::textBoxBackgroundColourId, brightMetalGrey);
	slider.setColour(juce::Slider::textBoxTextColourId, offWhite);
	slider.setColour(juce::Slider::rotarySliderOutlineColourId, metalGrey);
	slider.setColour(juce::Slider::rotarySliderFillColourId, brightMetalGrey);
	slider.setColour(juce::Slider::backgroundColourId, metalGrey);
	slider.setColour(juce::Slider::thumbColourId, offWhite);
	slider.setPopupDisplayEnabled(false, false, this);
}

void SulfuricAudioProcessorEditor::configureButton(juce::ShapeButton& button, bool isToggle)
{
	juce::Path smallCircle = juce::Path();
	smallCircle.addEllipse(0, 0, SMALL_BUTTON, SMALL_BUTTON);
	button.setShape(smallCircle, true, true, false);
	button.setClickingTogglesState(isToggle);
	button.setColours(brightMetalGrey, brightMetalGrey, isToggle ? brightMetalGrey : offWhite);
	button.setOnColours(offWhite, offWhite, offWhite);
	button.shouldUseOnColours(true);
}

uint64_t SulfuricAudioProcessorEditor::randomizeSeed()
{
	rng.setSeedRandomly();

	// Display the seed as if it were unsigned
	return (uint64_t)rng.getSeed();
}