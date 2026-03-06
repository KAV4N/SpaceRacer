#pragma once
#include "StrikeEngine.h"

class GameManager : public Strike::Script {
public:
    void onStart()                 override;
    void onUpdate(float deltaTime) override;

    Strike::Entity getLivesTextEntity() const { return mLivesText; }
    Strike::Entity getScoreTextEntity() const { return mScoreText; }

    float getRawAmplitude() const { return mRawAmplitude; }

    void notifyGameOver();

private:
    void initGameData();
    void updateHUD();
    void saveHighScore();
    void notifyPlayerWon();

    Strike::Entity mScoreText;
    Strike::Entity mLivesText;
    Strike::Entity mGameOverText;
    Strike::Entity mFinalScoreText;
    Strike::Entity mAmbientMusic;
    Strike::Entity mGameMusic;

    float mRawAmplitude      = 0.0f;
    float mMusicDuration     = 0.0f;
    float mMusicElapsed      = 0.0f;
    bool  mGameMusicPlaying  = false;
    bool  mGameEnded         = false;
};