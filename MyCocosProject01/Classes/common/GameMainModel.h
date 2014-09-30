//
//  GameMainModel.h
//  MyCocosProject01
//
//  Created by HiroyukiSasaki on 2014/09/06.
//
//

#ifndef __MyCocosProject01__GameMainModel__
#define __MyCocosProject01__GameMainModel__

#include <vector>
#include <random>


class GameMainModel
{
public:
	enum class GameStatus
	{
		Ready,
		Playing,
		TimeUp
	};

	struct BallInfo
	{
		int id;
		float initX;
		float initY;
		float x;
		float y;
		float impulseX;
		float impulseY;
		int number;
		bool isPresence;
		bool isSelectEnable;
		bool isSelected;
	};

	void receiveDtAndBallsPosition(const float dt, const std::vector<std::pair<float, float>> ballsPosition);
	void touchBall(const int id);
	void releaseBall();

	void enterDust();
	void outDust();


	void setGameStatus(GameStatus);
	GameStatus getGameStatus() const;
	GameStatus getGamePrevStatus() const;
	

	int getBallsNumber() const;
	std::vector<std::shared_ptr<BallInfo>> getBallsInfoList();
	std::vector<std::shared_ptr<BallInfo>> getSelectedBallsInfoList() const;
	std::vector<std::shared_ptr<BallInfo>> getVanishAnimationBallsInfoList() const;
	int getPresentPoints() const;
	int getAddingPoints() const;
	int getTotalPoints() const;
	bool isDustSelected() const;
	int getTimeLeft() const;
	void setCompleteVanishAnimation();

	GameMainModel();
	~GameMainModel();


private:

	std::mt19937 engine;
	std::uniform_int_distribution<> dist;

	GameStatus _gameStatus;
	GameStatus _gamePrevStatus;

	float _timeLeft;
	int _ballsNumber;

	std::vector<std::shared_ptr<BallInfo>> _ballsInfoList;
	std::vector<std::shared_ptr<BallInfo>> _selectedBallsInfoList;
	std::vector<std::shared_ptr<BallInfo>> _vanishAnimationBallsInfoList;

	bool _touchFlag;

	bool _dustSelectedFlag;

	int _presentPoints;
	int _totalPoints;

	void setUpGame();
	void createBallsInfoInInit();
	void addBallsInfo(const int addBallsNumber);
	void createBallInfo(const int id);
	void checkLengthSelectedBallToRest();
	void calculateTotalPoints();
	void allBallsDisabled();

	int _randomR;
};

#endif /* defined(__MyCocosProject01__GameMainModel__) */
