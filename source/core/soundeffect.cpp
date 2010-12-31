#include "soundeffect.h"

using namespace OSS;


//----------------------------------------------------------------------------------------------------
#pragma mark -
#pragma mark Initialization
//----------------------------------------------------------------------------------------------------

SoundEffect::SoundEffect()
{
	sourceID = 0;
	copyBeforeUse = false;
	maxConcurrentPlaybacks = 0;
	minIntervalBetweenPlaybacks = 0;
}

SoundEffect::SoundEffect(Sound * sound, Layer layer)
{
	sourceID = 0;
	copyBeforeUse = false;
	maxConcurrentPlaybacks = 0;
	minIntervalBetweenPlaybacks = 0;
	this->sound = sound;
	this->layer = layer;
}

SoundEffect::~SoundEffect()
{
	//Stop playback, this will also get rid of the source if necessary
	stop();
}

std::string SoundEffect::instanceName()
{
	return this->className() + " " + (sound ? sound->name : "<no sound>");
}





//----------------------------------------------------------------------------------------------------
#pragma mark -
#pragma mark Playback
//----------------------------------------------------------------------------------------------------

void SoundEffect::play()
{
	//If there's no source yet...
	if (!sourceID) {
		//Create the source
		alGenSources(1, &sourceID);
		
		//Setup some default source attributes
		alSourcei(sourceID, AL_BUFFER, sound->bufferID);
		alSourcei(sourceID, AL_SOURCE_RELATIVE, AL_TRUE);
	}
	
	//Start playing the source
	alSourcePlay(sourceID);
}

void SoundEffect::pause()
{
	if (!sourceID) return;
	
	//Pause the playback
	alSourcePause(sourceID);
}

void SoundEffect::stop()
{
	if (!sourceID) return;
	
	//Stop playing
	alSourceStop(sourceID);
	
	//Get rid of the source
	alDeleteSources(1, &sourceID);
	sourceID = 0;
}





//----------------------------------------------------------------------------------------------------
#pragma mark -
#pragma mark State
//----------------------------------------------------------------------------------------------------

ALint SoundEffect::getSourceState()
{
	ALint state;
	alGetSourcei(sourceID, AL_SOURCE_STATE, &state);
	return state;
}

bool SoundEffect::isPlaying()
{
	return (getSourceState() == AL_PLAYING);
}

bool SoundEffect::isStopped()
{
	return (getSourceState() == AL_STOPPED);
}

double SoundEffect::getSecondsPlayed()
{
	if (!sourceID || !sound) return 0;
	
	//WTF? alGetSourcef returns a float that without any fractional part! So we have to do the
	//magic ourselves.
	ALint samples;
	alGetSourcei(sourceID, AL_SAMPLE_OFFSET, &samples);
	ALint frequency;
	alGetBufferi(sound->bufferID, AL_FREQUENCY, &frequency);
	return ((double)samples / frequency);
}