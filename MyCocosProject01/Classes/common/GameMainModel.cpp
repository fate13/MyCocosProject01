//
//  GameMainModel.cpp
//  MyCocosProject01
//
//  Created by HiroyukiSasaki on 2014/09/06.
//
//

#include "GameConfig.h"
#include "GameMainModel.h"
#include <random>

GameMainModel::~GameMainModel()
{
	_ballsInfoList.clear();
	_selectedBallsInfoList.clear();
	_vanishAnimationBallsInfoList.clear();
}

GameMainModel::GameMainModel()
{
	std::random_device rdev;
	std::mt19937 engine(rdev());
	std::uniform_int_distribution<> dist(0, 359);
	_randomR = dist(engine);

	setUpGame();
}

void GameMainModel::setUpGame()
{
	_gamePrevStatus = GameStatus::Ready;
	_gameStatus = GameStatus::Ready;
	_timeLeft = DEFAULT_TIME_LEFT;
	_ballsNumber = BALLS_NUMBER;
	_touchFlag = false;
	_dustSelectedFlag = false;
	_presentPoints = 0;
	_totalPoints = 0;


	createBallsInfoInInit();
}

void GameMainModel::createBallsInfoInInit()
{
	for (int i = 0; i < _ballsNumber; ++i) {
		createBallInfo(i);
	}
}

void GameMainModel::addBallsInfo(const int addBallsNumber)
{
	for (int i = _ballsNumber; i < _ballsNumber + addBallsNumber; ++i) {
		createBallInfo(i);
	}
	_ballsNumber += addBallsNumber;
}


void GameMainModel::createBallInfo(const int id)
{
	std::random_device rdev;
	std::mt19937 engine(rdev());
	std::uniform_int_distribution<> dist(0, 99);

	std::shared_ptr<BallInfo> ballInfo = std::make_shared<BallInfo>();
	ballInfo->id = id;

	if (id < BALLS_NUMBER)
	{
		ballInfo->initX = 0.5f + (0.25f + (float)id * 0.01) * cosf((float)(id + _randomR) * 44.0f * MATH_PI / 180.0f);
		ballInfo->initY = 0.5f + (0.25f + (float)id * 0.01) * sinf((float)(id + _randomR) * 44.0f * MATH_PI / 180.0f) * 9.0f / 16.0f;
	}
	else
	{
		ballInfo->initX = 0.5f + 0.7f * ((dist(engine) % 2)-0.5f) * 2.0f;
		ballInfo->initY = 0.5f + 0.2f * ((dist(engine) % 2)-0.5f) * 2.0f;
	}

	ballInfo->x = ballInfo->initX;
	ballInfo->y = ballInfo->initY;


	int randomNumber = dist(engine);

	if (randomNumber < NUMBER_1_PERCENT)
		ballInfo->number = 1;
	else if (randomNumber < NUMBER_2_PERCENT)
		ballInfo->number = 2;
	else if (randomNumber < NUMBER_3_PERCENT)
		ballInfo->number = 3;
	else if (randomNumber < NUMBER_4_PERCENT)
		ballInfo->number = 4;
	else if (randomNumber < NUMBER_5_PERCENT)
		ballInfo->number = 5;
	else if (randomNumber < NUMBER_6_PERCENT)
		ballInfo->number = 6;
	else if (randomNumber < NUMBER_7_PERCENT)
		ballInfo->number = 7;
	else if (randomNumber < NUMBER_8_PERCENT)
		ballInfo->number = 8;
	else
		ballInfo->number = 9;


	ballInfo->isPresence = true;
	ballInfo->isSelectEnable = true;
	ballInfo->isSelected = false;

	_ballsInfoList.push_back(ballInfo);
}

void GameMainModel::receiveDtAndBallsPosition(const float dt, const std::vector<std::pair<float, float>> ballsPosition)
{
	_timeLeft -= dt;

	int size = ballsPosition.size();
	for (int i = 0; i < size; ++i) {
		_ballsInfoList.at(i)->x = ballsPosition.at(i).first;
		_ballsInfoList.at(i)->y = ballsPosition.at(i).second;
	}

	if (_touchFlag)
		checkLengthSelectedBallToRest();

	if (_timeLeft < 0.0f) {
		setGameStatus(GameStatus::TimeUp);
		_timeLeft = 0.0f;
	}
	else if (_timeLeft < DEFAULT_TIME_LEFT)
	{
		setGameStatus(GameStatus::Playing);
	}
}

void GameMainModel::touchBall(const int id)
{
	_touchFlag = true;

	if (!_ballsInfoList.at(id)->isSelectEnable)
		return;

	if (_ballsInfoList.at(id)->isSelected)
	{
		if (_selectedBallsInfoList.size() > 1)
		{
			int size = _selectedBallsInfoList.size();
			for (int i = size - 1; i > -1; --i)
			{
				if (_selectedBallsInfoList.at(i)->id == _ballsInfoList.at(id)->id)
				{
					return;
				}
				else
				{
					_ballsInfoList.at(_selectedBallsInfoList.at(i)->id)->isSelected = false;
					_presentPoints -= _selectedBallsInfoList.at(i)->number;
					_selectedBallsInfoList.pop_back();
				}
			}
		}
	}
	else // _presentPointsに加算
	{
		_ballsInfoList.at(id)->isSelected	= true;
		_presentPoints += _ballsInfoList.at(id)->number;
		_selectedBallsInfoList.push_back(_ballsInfoList.at(id));
	}
}

void GameMainModel::releaseBall()
{
	_touchFlag = false;

	_vanishAnimationBallsInfoList.clear();

	int addBallsNumber = 0;

	int size = _ballsInfoList.size();

	// 複数選択状態のボールを決定、_totalPointsに加算
	if (_selectedBallsInfoList.size() >= 2)
	{
		for (int i = 0; i < size; ++i)
		{

			if (_ballsInfoList.at(i)->isSelected && _ballsInfoList.at(i)->isPresence)
			{
				// １０点の場合、すべて消滅
				if (_presentPoints == 10) {
					_ballsInfoList.at(i)->isPresence	= false;
					++addBallsNumber;
				}
				// １０点未満の場合、最後の一個のnumberに加算
				else
				{
					// 最後の一個
					if (_selectedBallsInfoList.at(_selectedBallsInfoList.size() - 1)->id == _ballsInfoList.at(i)->id)
					{
						_ballsInfoList.at(i)->number = _presentPoints;
					}
					// それ以外
					else
					{
						_ballsInfoList.at(i)->isPresence	= false;
						++addBallsNumber;
					}
				}
				_ballsInfoList.at(i)->isSelected	= false;
			}
		}
		_vanishAnimationBallsInfoList = _selectedBallsInfoList;
		calculateTotalPoints();
	}
	// １個だけ選択状態をキャンセル、ダストのときは消滅
	else
	{
		for (int i = 0; i < size; ++i) {

			if (_ballsInfoList.at(i)->isSelected)
			{
				if (_dustSelectedFlag)
				{
					_ballsInfoList.at(i)->isPresence = false;
					_dustSelectedFlag = false;
					++addBallsNumber;
					_vanishAnimationBallsInfoList = _selectedBallsInfoList;
				}

				_ballsInfoList.at(i)->isSelected	= false;
			}
		}
	}

	_presentPoints = 0;
	_selectedBallsInfoList.clear();

	for (int i = 0; i < _ballsNumber; ++i)
	{
		if (_ballsInfoList.at(i)->isPresence && !_ballsInfoList.at(i)->isSelectEnable)
			_ballsInfoList.at(i)->isSelectEnable = true;
	}

	if (addBallsNumber > 0)
		addBallsInfo(addBallsNumber);
}

void GameMainModel::calculateTotalPoints()
{
	if (_gameStatus == GameStatus::TimeUp)
		return;
	
	if (_presentPoints == 10)
		_totalPoints += (_presentPoints * _selectedBallsInfoList.size());
	else
		_totalPoints += _presentPoints;
}

void GameMainModel::checkLengthSelectedBallToRest()
{
	std::shared_ptr<BallInfo> lastSelectedBollInfo = _selectedBallsInfoList.at(_selectedBallsInfoList.size() - 1);
	float lX = lastSelectedBollInfo->x;
	float lY = lastSelectedBollInfo->y;

	for (int i = 0; i < _ballsNumber; ++i) {

		if (!_ballsInfoList.at(i)->isPresence && _ballsInfoList.at(i)->isSelected)
			continue;

		float iX = _ballsInfoList.at(i)->x;
		float iY = _ballsInfoList.at(i)->y;

		float distance = sqrtf(powf((iX-lX), 2) + powf((iY-lY), 2));

		if (distance > BALL_DISTANCE_DISABLED_THRESHOLD && !_ballsInfoList.at(i)->isSelected)
			_ballsInfoList.at(i)->isSelectEnable = false;
		else
		{
			if (_ballsInfoList.at(i)->number + _presentPoints > 10 && !_ballsInfoList.at(i)->isSelected)
				_ballsInfoList.at(i)->isSelectEnable = false;
			else
				_ballsInfoList.at(i)->isSelectEnable = true;
		}

	}
}

void GameMainModel::enterDust()
{
	if (_dustSelectedFlag)
		return;

	_dustSelectedFlag = true;

	if (_selectedBallsInfoList.size() > 1)
	{
		std::shared_ptr<BallInfo> firstSelected = _selectedBallsInfoList.at(0);
		_selectedBallsInfoList.clear();
		_selectedBallsInfoList.push_back(firstSelected);
		_presentPoints = firstSelected->number;

		for (int i = 0; i < _ballsNumber; ++i)
		{
			if (_ballsInfoList.at(i)->id != firstSelected->id)
				_ballsInfoList.at(i)->isSelected = false;
		}
	}
}

void GameMainModel::outDust()
{
	if (!_dustSelectedFlag)
		return;

	_dustSelectedFlag = false;
}

void GameMainModel::setCompleteVanishAnimation()
{
	_vanishAnimationBallsInfoList.clear();
}

void GameMainModel::setGameStatus(GameStatus status)
{
	_gamePrevStatus = _gameStatus;
	_gameStatus = status;
}

GameMainModel::GameStatus GameMainModel::getGameStatus() const
{
	return _gameStatus;
}

GameMainModel::GameStatus GameMainModel::getGamePrevStatus() const
{
	return _gamePrevStatus;
}

int GameMainModel::getBallsNumber() const
{
	return _ballsNumber;
}

std::vector<std::shared_ptr<GameMainModel::BallInfo>> GameMainModel::getBallsInfoList()
{
	return _ballsInfoList;
}

std::vector<std::shared_ptr<GameMainModel::BallInfo>> GameMainModel::getSelectedBallsInfoList() const
{
	return _selectedBallsInfoList;
}

std::vector<std::shared_ptr<GameMainModel::BallInfo>> GameMainModel::getVanishAnimationBallsInfoList() const
{
	return _vanishAnimationBallsInfoList;
}

bool GameMainModel::isDustSelected() const
{
	return _dustSelectedFlag;
}

int GameMainModel::getPresentPoints() const
{
	return _presentPoints;
}

int GameMainModel::getTotalPoints() const
{
	return _totalPoints;
}

int GameMainModel::getTimeLeft() const
{
	return ceilf(_timeLeft);
}




