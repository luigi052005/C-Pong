#include <stdio.h>
#include <string>
#include <iostream>
#include <random>
#include <raylib.h>

struct Ball
{
	float x, y;
	float speedX, speedY; 
	float startSpeed; 
	float minSpeed;
	float maxSpeed;
	float radius;
	Color color;

	void Draw()
	{
		DrawCircle((int)x, (int)y, radius, color);
	}
};

struct Paddle
{
	int score;
	float x, y;
	float speed; 
	float width, height;
	bool bot;
	Color color;

	Rectangle GetRect()
	{
		return Rectangle{ x - width / 2, y - height / 2, 10, 100 };
	}

	void Draw()
	{
		DrawRectangleRec(GetRect(), color);
	}
};

Ball ball;
Paddle leftPaddle;
Paddle rightPaddle;
const char* winner = nullptr;
bool vsync = false;

//MATH
float lerp(float start, float end, float t)
{
	return start + t * (end - start);
}

float clamp(float value, float minValue, float maxValue) {
	return std::max(minValue, std::min(value, maxValue));
}

float randomDirection(float speed)
{
	int r = GetRandomValue(1, 2);
	if (r > 1)
	{
		return speed * -1;
	}
	else {
		return speed;
	}
}
//SETVARS
void initialization()
{
	winner = nullptr;

	ball.x = GetScreenWidth() / 2.0f;
	ball.y = GetScreenHeight() / 2.0f;
	ball.radius = 5;
	ball.startSpeed = 350;
	ball.speedX = randomDirection(ball.startSpeed);
	ball.speedY = randomDirection(ball.startSpeed);
	ball.minSpeed = -150000;
	ball.maxSpeed = 150000;
	ball.color = WHITE;

	leftPaddle.x = 50.0f;
	leftPaddle.y = GetScreenHeight() / 2;
	leftPaddle.width = 10;
	leftPaddle.height = 100;
	leftPaddle.speed = 500;
	leftPaddle.color = WHITE;

	rightPaddle.x = GetScreenWidth() - 50;
	rightPaddle.y = GetScreenHeight() / 2;
	rightPaddle.width = 10;
	rightPaddle.height = 100;
	rightPaddle.color = WHITE;
}

//BOT
void paddleBot()
{
	if (rightPaddle.bot)
		rightPaddle.y = lerp(rightPaddle.y, ball.y, 10.00f * GetFrameTime());
	if (leftPaddle.bot)
		leftPaddle.y = lerp(leftPaddle.y, ball.y, 10.00f * GetFrameTime());
}

//INPUT
void toggle()
{
	if (IsKeyPressed(KEY_B))
	{
		rightPaddle.bot = !rightPaddle.bot;
	}
	if (IsKeyPressed(KEY_N))
	{
		leftPaddle.bot = !leftPaddle.bot;
	}
	if (IsKeyPressed(KEY_V))
	{
		vsync = !vsync;
	}
	if (vsync)
		SetWindowState(FLAG_VSYNC_HINT);
	else 
		ClearWindowState(FLAG_VSYNC_HINT);

}

void leftPlayer()
{
	if (!leftPaddle.bot)
	{
		if (IsKeyDown(KEY_W))
		{
			leftPaddle.y -= leftPaddle.speed * GetFrameTime();
		};
		if (IsKeyDown(KEY_S))
		{
			leftPaddle.y += leftPaddle.speed * GetFrameTime();
		};
	}
	leftPaddle.y = clamp(leftPaddle.y, 50, GetScreenHeight() - 50);
}

void rightPlayer()
{
	if (!rightPaddle.bot)
	{
		if (IsKeyDown(KEY_UP))
		{
			rightPaddle.y -= leftPaddle.speed * GetFrameTime();
		};
		if (IsKeyDown(KEY_DOWN))
		{
			rightPaddle.y += leftPaddle.speed * GetFrameTime();
		};
	}
	rightPaddle.y = clamp(rightPaddle.y, 50, GetScreenHeight() - 50);
}

//MOVEBALL
void moveBall()
{
	//MOVEBALL
	ball.x += ball.speedX * GetFrameTime();
	ball.y += ball.speedY * GetFrameTime();
}

//COLLISION
void collosion()
{
	if (ball.y > GetScreenHeight() - ball.radius / 2)
	{
		ball.y = GetScreenHeight() - ball.radius / 2;
		ball.speedY *= -1;
	}
	if (ball.y < 0 + ball.radius / 2)
	{
		ball.y = 0 + ball.radius / 2;
		ball.speedY *= -1;
	}
	if (CheckCollisionCircleRec(Vector2{ ball.x, ball.y }, ball.radius, leftPaddle.GetRect()))
	{
		if (ball.speedX < 0)
		{
			ball.speedX *= -1.05f;
			ball.speedY = (ball.y - leftPaddle.y) / (leftPaddle.height / 2) * ball.speedX;
			leftPaddle.speed += 50;
		}
		if (ball.speedX > ball.maxSpeed)
		{
			ball.speedX = ball.maxSpeed;
		}
	}

	if (CheckCollisionCircleRec(Vector2{ ball.x, ball.y }, ball.radius, rightPaddle.GetRect()))
	{
		if (ball.speedX > 0)
		{
			ball.speedX *= -1.05f;
			ball.speedY = (ball.y - rightPaddle.y) / (rightPaddle.height / 2) * -ball.speedX;
			rightPaddle.speed += 50;
		}
		if (ball.speedX > ball.maxSpeed)
		{
			ball.speedX = ball.maxSpeed;
		}
	}
}

//WINNING
void winning()
{
	if (rightPaddle.score >= 5)
	{
		if (rightPaddle.bot && leftPaddle.bot)
			winner = "Bot 1 hat won!";
		else if (rightPaddle.bot == true && !leftPaddle.bot)
			winner = "You lost to a bot, noob!";
		else
			winner = "Player 1 has won!";
	}
	else if (leftPaddle.score >= 5)
	{
		if (leftPaddle.bot && rightPaddle.bot)
			winner = "Bot 2 hat won!";
		else if (leftPaddle.bot == true && !rightPaddle.bot)
			winner = "You lost to a bot, fool!";
		else
			winner = "Player 2 has won!";
	}
}

//SCORE
void score()
{
	if (ball.x < 0)
	{
		rightPaddle.score += 1;
		initialization();
		return;
	}
	if (ball.x > GetScreenWidth())
	{
		leftPaddle.score += 1;
		initialization();
		return;
	}
}

//RESET
void reset()
{
	if (winner && IsKeyDown(KEY_SPACE))
	{
		initialization();
		rightPaddle.score = 0;
		leftPaddle.score = 0;
	}
}

//RENDER
void render()
{
	//RENDER
	BeginDrawing();
	ClearBackground(BLACK);
	if (IsWindowResized())
	{
		rightPaddle.x = GetScreenWidth() - 50;
	}
	ball.Draw();
	leftPaddle.Draw();
	rightPaddle.Draw();

	if (winner)
	{
		int textWidth = MeasureText(winner, 60);
		DrawText(winner, GetScreenWidth() / 2 - textWidth / 2, GetScreenHeight() / 2 - 30, 60, WHITE);
		leftPaddle.color = BLACK;
		rightPaddle.color = BLACK;
		ball.color = BLACK;
	}
	else
	{
		std::string scoreString = std::to_string(leftPaddle.score) + " - " + std::to_string(rightPaddle.score);
		int textWidth = MeasureText(scoreString.c_str(), 50);
		DrawText(scoreString.c_str(), GetScreenWidth() / 2 - textWidth / 2, 0, 50, WHITE);
		//DrawText(std::to_string(leftPaddle.y).c_str(), GetScreenWidth() / 2 - textWidth / 2, GetScreenHeight() / 2 + 100, 30, WHITE); DEBUG
	}

	DrawFPS(10, 10);
	EndDrawing();
}

//GAMELOOP
int main()
{	
	InitWindow(1200, 900, "Pong!");
	//SetTargetFPS(1500);
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	rightPaddle.bot = true;

	initialization();

	while (!WindowShouldClose())
	{
		leftPlayer();
		rightPlayer();
		paddleBot();
		toggle();
		moveBall();
		collosion();
		winning();
		score();
		reset();
		render();
	}

	CloseWindow();

	return 0;
}
