#include <Siv3D.hpp> // OpenSiv3D v0.4.3
#include <cmath>
#include <ctime>
#include <vector>
#include <time.h>
#include <stdio.h>
using namespace std;
#pragma warning (disable: 4996)

struct Ball {
	int isvalid = 0;
	int ty = 0;
	double px, py, vx, vy;
};

struct Ranking {
	int Score;
	int v1, v2, v3, v4, v5;
};

bool operator<(const Ranking& a1, const Ranking& a2) {
	if (a1.Score > a2.Score) return true;
	if (a1.Score < a2.Score) return false;
	if (a1.v1 > a2.v1) return true;
	if (a1.v1 < a2.v1) return false;
	if (a1.v2 > a2.v2) return true;
	if (a1.v2 < a2.v2) return false;
	if (a1.v3 > a2.v3) return true;
	if (a1.v3 < a2.v3) return false;
	if (a1.v4 > a2.v4) return true;
	if (a1.v4 < a2.v4) return false;
	if (a1.v5 > a2.v5) return true;
	return false;
}

bool operator==(const Ranking& a1, const Ranking& a2) {
	if (a1.Score != a2.Score) return false;
	if (a1.v1 != a2.v1) return false;
	if (a1.v2 != a2.v2) return false;
	if (a1.v3 != a2.v3) return false;
	if (a1.v4 != a2.v4) return false;
	if (a1.v5 != a2.v5) return false;
	return true;
}

// -------------------------------- 変数 -----------------------------------
const double SEIGEN = 150;
const double BALLRATE = 5.0;
const double w1[10] = { 0.7, 0.7, 0.7, 0.7, 0.7, 0.6, 0.6, 0.6, 0.5, 0.5 };
const double w2[10] = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.3, 0.3, 0.3, 0.3, 0.3 };
const double w3[10] = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.2, 0.2 };
const double iro1[4] = { 0.0, 1.0, 1.0, 0.5 };
const double iro2[4] = { 0.0, 1.0, 0.0, 0.5 };
const double iro3[4] = { 0.0, 0.0, 0.0, 1.0 };

// -------------------------- スイッチ用の変数 -----------------------------
double GetLastClick;
double Click_A[100];
double Click_B[6][6]; bool LineState_B[6][6];
double Click_C[6][6]; bool LineState_C[6][6];
double Click_D;

// --------------------------- ゲーム用の変数 ------------------------------
int Situation = 1, NumPage = 1, CurrentScore;
int ScoreGraph[2000];
double Elapsed = 0.0;
Ball B[2000];

// ------------------------------- 順位表 ----------------------------------
pair<String, int> Ranker[22] = {
	make_pair(U"F", 0), make_pair(U"C", 50), make_pair(U"C+", 60),
	make_pair(U"B-", 65), make_pair(U"B", 70), make_pair(U"B+", 75),
	make_pair(U"A-", 80), make_pair(U"A", 85), make_pair(U"A+", 90),
	make_pair(U"S", 95), make_pair(U"SS", 100), make_pair(U"SSS", 105),
	make_pair(U"Champion", 110), make_pair(U"Genius", 116), make_pair(U"Prodigy", 123),
	make_pair(U"Master", 130), make_pair(U"Grandmaster", 138), make_pair(U"Legend", 147),
	make_pair(U"Superhuman", 157), make_pair(U"Jeff-Bezos", 168), make_pair(U"God", 180),
	make_pair(U"Ultimate-God", 200)
};
int NumPlays = 0;
Ranking Rank[1 << 15];
Ranking YourScore;

int Rand() {
	int s = 0, t = 1;
	for (int i = 0; i < 6; i++) {
		s += t * (rand() % 32);
		t *= 32;
	}
	return s;
}

double Randouble() {
	double s = 0, t = 1;
	for (int i = 0; i < 6; i++) {
		t /= 1024.0;
		s += 1.0 * (rand() % 1024) * t;
	}
	return s;
}

void GetTime(Ranking &RR) {
	time_t now = std::time(nullptr);
	struct tm* localNow = std::localtime(&now);
	RR.v1 = localNow->tm_mon + 1;
	RR.v2 = localNow->tm_mday;
	RR.v3 = localNow->tm_hour;
	RR.v4 = localNow->tm_min;
	RR.v5 = localNow->tm_sec;
}

String GetRankToScore(int x) {
	int id = 0;
	for (int i = 0; i < 22; i++) {
		if (Ranker[i].second <= x) id = i;
	}
	return Ranker[id].first;
}

int RankToNext(int x) {
	int id = 0;
	for (int i = 0; i < 22; i++) {
		if (Ranker[i].second <= x) id = i;
	}
	return Ranker[id + 1].second - x;
}

void get_Ranking() {
	FILE* in = freopen("ranking.txt", "r", stdin);
	cin >> NumPlays;
	for (int i = 0; i < NumPlays; i++) {
		cin >> Rank[i].Score >> Rank[i].v1 >> Rank[i].v2 >> Rank[i].v3 >> Rank[i].v4 >> Rank[i].v5;
	}
}

void output_Ranking() {
	FILE* in = freopen("ranking.txt", "w", stdout);
	cout << NumPlays << endl;
	for (int i = 0; i < NumPlays; i++) {
		cout << Rank[i].Score << " " << Rank[i].v1 << " " << Rank[i].v2 << " " << Rank[i].v3 << " " << Rank[i].v4 << " " << Rank[i].v5 << endl;
	}
}

void init_all() {
	CurrentScore = 0; Elapsed = 0.0;
	for (int i = 0; i < 100; i++) Click_A[i] = 0;
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 6; j++) {
			Click_B[i][j] = 0; LineState_B[i][j] = false;
			Click_C[i][j] = 0; LineState_C[i][j] = false;
		}
	}
	for (int i = 0; i < 2000; i++) {
		ScoreGraph[i] = 0;
		B[i].isvalid = 0; B[i].ty = 0;
		B[i].px = 0; B[i].py = 0; B[i].vx = 0; B[i].vy = 0;
	}
}

void initialize() {
	Elapsed = -4.0;
	for (int t = 0; t < 10; t++) {
		int kosuu = (int)(0.1 * (SEIGEN - 10.0) * BALLRATE + 0.5);

		// ボールの位置を決める
		int cl = kosuu * t, cr = kosuu * (t + 1);
		for (int i = cl; i < cr; i++) {
			B[i].isvalid = 0;
			while (true) {
				int ty = Rand() % 4;
				if (ty == 0) { B[i].px = 0; B[i].py = Randouble() * 480.0 + 60.0; }
				if (ty == 1) { B[i].px = 600; B[i].py = Randouble() * 480.0 + 60.0; }
				if (ty == 2) { B[i].py = 0; B[i].px = Randouble() * 480.0 + 60.0; }
				if (ty == 3) { B[i].py = 600; B[i].px = Randouble() * 480.0 + 60.0; }
				double rr = pow(4.0, Randouble()) * 84.0;
				double theta = Randouble() * 2.0 * 3.14159265358979;
				B[i].vx = rr * cos(theta);
				B[i].vy = rr * sin(theta);
				double proj_x = B[i].px + 0.2 * B[i].vx;
				double proj_y = B[i].py + 0.2 * B[i].vy;
				if (proj_x < 0.0 || proj_x > 600.0 || proj_y < 0.0 || proj_y > 600.0) continue;
				if (abs(B[i].vx) < 6.0 || abs(B[i].vy) < 6.0) continue;
				break;
			}
		}

		// ボールの色を決める
		for (int i = cl; i < cl + (int)(1.0 * w1[t] * kosuu); i++) B[i].ty = 1;
		for (int i = cl + (int)(1.0 * w1[t] * kosuu); i < cl + (int)(1.0 * (w1[t] + w2[t]) * kosuu); i++) B[i].ty = 2;
		for (int i = cl + (int)(1.0 * (w1[t] + w2[t]) * kosuu); i < cr; i++) B[i].ty = 3;
		for (int i = 0; i < 1000; i++) swap(B[cl + rand() % (cr - cl)].ty, B[cl + rand() % (cr - cl)].ty);
	}
}

void move_balls(double Delta) {
	for (int i = 0; i < 1000; i++) {
		if (B[i].isvalid == 0 && Elapsed >= 1.0 * i / BALLRATE) B[i].isvalid = 1;
		if (B[i].isvalid != 1) continue;
		if (Elapsed < SEIGEN) {
			for (int j = 0; j < 20; j++) {
				B[i].px += Delta * B[i].vx / 20.0;
				B[i].py += Delta * B[i].vy / 20.0;
				if (B[i].py >= 70.0 && B[i].py <= 530.0 && B[i].px >= 100.0 && B[i].px <= 500.0) {
					int idx_a = (int)((B[i].py - 60.0) / 80.0); idx_a = min(5, max(0, idx_a));
					int idx_b = (int)((B[i].px - 100.0) / 80.0); idx_b = min(4, max(0, idx_b));
					double dist1 = abs((B[i].py - Delta * B[i].vy / 20.0) -(100.0 + 80.0 * idx_a));
					double dist2 = abs(B[i].py - (100.0 + 80.0 * idx_a));
					if (LineState_B[idx_a][idx_b] == true && dist2 < 4.0 && dist2 < dist1) B[i].vy *= -1;
				}
				if (B[i].px >= 70.0 && B[i].px <= 530.0 && B[i].py >= 100.0 && B[i].py <= 500.0) {
					int idx_b = (int)((B[i].px - 60.0) / 80.0); idx_b = min(5, max(0, idx_b));
					int idx_a = (int)((B[i].py - 100.0) / 80.0); idx_a = min(4, max(0, idx_a));
					double dist1 = abs((B[i].px - Delta * B[i].vx / 20.0) - (100.0 + 80.0 * idx_b));
					double dist2 = abs(B[i].px - (100.0 + 80.0 * idx_b));
					if (LineState_C[idx_a][idx_b] == true && dist2 < 4.0 && dist2 < dist1) B[i].vx *= -1;
				}
			}
		}
		if (B[i].px < -100.0 || B[i].px > 700.0 || B[i].py < -100.0 || B[i].py > 700.0) {
			B[i].isvalid = 2;
		}
	}
}

void Main() {
	srand((unsigned)time(NULL));

	// 背景を黒にする
	Scene::SetBackground(ColorF(0.0, 0.0, 0.1));

	// フォントを用意
	const Font font80(80);
	const Font font70(70);
	const Font font60(60);
	const Font font50(50);
	const Font font40(40);
	const Font font35(35);
	const Font font30(30);
	const Font font25(25);
	const Font font20(20);
	const Font font18(18);
	const Font font15(15);
	const Font font10(10);

	// ランキングの入手
	get_Ranking();

	while (System::Update()) {
		double MouseX = Cursor::PosF().x;
		double MouseY = Cursor::PosF().y;

		// --------------------------------------- Step #1. 待ち受け画面 -------------------------------------
		if (Situation == 1) {
			font80(U"Valuable Balls").draw(120, 45);
			font35(U"～E869120 作成のボール集めゲーム～").draw(80, 150);

			Rect(100, 250, 270, 80).draw(ColorF(Click_A[0], 0.0, 0.0));
			Rect(430, 250, 270, 80).draw(ColorF(Click_A[1], 0.0, 0.0));
			Rect(100, 360, 270, 80).draw(ColorF(Click_A[2], 0.0, 0.0));
			Rect(430, 360, 270, 80).draw(ColorF(Click_A[3], 0.0, 0.0));
			font40(U"やってみる").draw(135, 262, ColorF(0.0, 0.0, 0.0));
			font40(U"遊び方").draw(505, 262, ColorF(0.0, 0.0, 0.0));
			font40(U"ランキング").draw(135, 372, ColorF(0.0, 0.0, 0.0));
			font40(U"おわる").draw(505, 372, ColorF(0.0, 0.0, 0.0));
			font30(U"ボタンをクリックしてください").draw(190, 490, ColorF(Periodic::Sine0_1(1.5s)));

			int idx = -1;
			if (MouseX >= 100.0 && MouseX <= 370.0 && MouseY >= 250.0 && MouseY <= 330.0) idx = 0;
			if (MouseX >= 430.0 && MouseX <= 700.0 && MouseY >= 250.0 && MouseY <= 330.0) idx = 1;
			if (MouseX >= 100.0 && MouseX <= 370.0 && MouseY >= 360.0 && MouseY <= 440.0) idx = 2;
			if (MouseX >= 430.0 && MouseX <= 700.0 && MouseY >= 360.0 && MouseY <= 440.0) idx = 3;
			if (idx != 0) Click_A[0] = max(0.3, Click_A[0] - 0.04); else Click_A[0] = min(0.9, Click_A[0] + 0.04);
			if (idx != 1) Click_A[1] = max(0.3, Click_A[1] - 0.04); else Click_A[1] = min(0.9, Click_A[1] + 0.04);
			if (idx != 2) Click_A[2] = max(0.3, Click_A[2] - 0.04); else Click_A[2] = min(0.9, Click_A[2] + 0.04);
			if (idx != 3) Click_A[3] = max(0.3, Click_A[3] - 0.04); else Click_A[3] = min(0.9, Click_A[3] + 0.04);

			if (Scene::Time() - GetLastClick >= 0.1 && MouseL.down()) {
				GetLastClick = Scene::Time();
				if (idx == 0) { Situation = 2; initialize(); }
				if (idx == 1) Situation = 3;
				if (idx == 2) Situation = 4;
				if (idx == 3) break;
			}
		}

		// --------------------------------------- Step #2. ゲーム画面 -------------------------------------
		if (Situation == 2) {
			Elapsed += Scene::DeltaTime();
			move_balls(Scene::DeltaTime());

			// 盤面の表示
			for (int i = 0; i < 1000; i++) {
				if (B[i].isvalid != 1) continue;
				Circle(B[i].px, B[i].py, 5).draw(ColorF(iro1[B[i].ty], iro2[B[i].ty], iro3[B[i].ty]));
			}
			for (int i = 0; i < 6; i++) {
				for (int j = 0; j < 6; j++) {
					if (j != 5) {
						double c1 = 0.0, c2 = 0.0, c3 = 0.0;
						if (LineState_B[i][j] == true) {
							c1 = 1.0;
							c2 = 1.0;
							c3 = 1.0 - Click_B[i][j];
						}
						else {
							c1 = 0.1 + 0.3 * Click_B[i][j];
							c2 = 0.1 + 0.3 * Click_B[i][j];
							c3 = 0.19;
						}
						Line(100 + 80 * j, 100 + 80 * i, 180 + 80 * j, 100 + 80 * i).draw(4, ColorF(c1, c2, c3));
					}
					if (i != 5) {
						double c1 = 0.0, c2 = 0.0, c3 = 0.0;
						if (LineState_C[i][j] == true) {
							c1 = 1.0;
							c2 = 1.0;
							c3 = 1.0 - Click_C[i][j];
						}
						else {
							c1 = 0.1 + 0.3 * Click_C[i][j];
							c2 = 0.1 + 0.3 * Click_C[i][j];
							c3 = 0.19;
						}
						Line(100 + 80 * j, 100 + 80 * i, 100 + 80 * j, 180 + 80 * i).draw(4, ColorF(c1, c2, c3));
					}
				}
			}
			
			// スコア計算
			CurrentScore = 0;
			for (int i = 0; i < 2000; i++) {
				if (B[i].isvalid != 1) continue;
				if (!(B[i].px >= 0.0 && B[i].px <= 600.0 && B[i].py >= 0.0 && B[i].py <= 600.0)) continue;
				if (B[i].ty == 1) CurrentScore += 2;
				if (B[i].ty == 2) CurrentScore -= 2;
				if (B[i].ty == 3) CurrentScore += 5;
			}
			CurrentScore = max(CurrentScore, 0);
			ScoreGraph[min((int)(SEIGEN + 0.1), (int)(Elapsed + 1.0))] = CurrentScore;

			// ステータスバーの表示
			Rect(600, 0, 200, 100).draw(ColorF(0.2, 0.0, 0.0));
			Rect(600, 100, 200, 500).draw(ColorF(0.3, 0.0, 0.0));
			font25(U"VALUABLE").draw(620, 15);
			font25(U"BALLS").draw(700, 50);
			font25(U"スコア").draw(620, 120);
			if (CurrentScore < 10) font50(CurrentScore).draw(725, 155);
			else if (CurrentScore < 100) font50(CurrentScore).draw(695, 155);
			else font50(CurrentScore).draw(665, 155);
			font25(U"点").draw(765, 180);

			int ActualElapsed = min((int)SEIGEN, max(0, (int)(SEIGEN - Elapsed + 1.0)));
			double wariai = min(1.0, max(0.0, (1.0 * SEIGEN - Elapsed) / SEIGEN));
			font25(U"残り時間").draw(620, 360);
			if (ActualElapsed <= 20) {
				Circle(700, 490, 80).draw(ColorF(1.0, 0.0, 0.0, 0.2));
				Circle(700, 490, 80).drawPie(0, ToRadians(wariai * 360.0), ColorF(1.0, 0.0, 0.0));
				Circle(700, 490, 60).draw(ColorF(0.3, 0.0, 0.0));
			}
			else if (ActualElapsed <= 50) {
				Circle(700, 490, 80).draw(ColorF(1.0, 1.0, 0.0, 0.2));
				Circle(700, 490, 80).drawPie(0, ToRadians(wariai * 360.0), ColorF(1.0, 1.0, 0.0));
				Circle(700, 490, 60).draw(ColorF(0.3, 0.0, 0.0));
			}
			else {
				Circle(700, 490, 80).draw(ColorF(0.5, 0.5, 1.0, 0.2));
				Circle(700, 490, 80).drawPie(0, ToRadians(wariai * 360.0), ColorF(0.5, 0.5, 1.0));
				Circle(700, 490, 60).draw(ColorF(0.3, 0.0, 0.0));
			}
			if (ActualElapsed < 10) font40(ActualElapsed).draw(686, 463);
			else if (ActualElapsed < 100) font40(ActualElapsed).draw(674, 463);
			else font40(ActualElapsed).draw(662, 463);


			// 開始前の場合
			if (Elapsed < 0.0) {
				int NextGoal = 0;
				int nc1[3] = { 255, 255, 255 };
				int nc2[3] = { 0, 127, 255 };
				int nc3[3] = { 0, 0, 0 };
				if (Elapsed < -3.0) NextGoal = 3;
				else if (Elapsed < -2.0) NextGoal = 2;
				else if (Elapsed < -1.0) NextGoal = 1;
				else if (Elapsed < -0.0) NextGoal = 0;
				if (NextGoal < 3) {
					Circle(300, 300, 130).draw(Color(nc1[NextGoal], nc2[NextGoal], nc3[NextGoal], 40));
					Circle(300, 300, 130).drawPie(0, ToRadians((-Elapsed - 1.0 * NextGoal) * 360.0), Color(nc1[NextGoal], nc2[NextGoal], nc3[NextGoal]));
					Circle(300, 300, 100).draw(ColorF(0.0, 0.0, 0.1));
					font80(NextGoal + 1).draw(270, 250);
				}
			}

			// 終了後の場合
			if (Elapsed >= SEIGEN) {
				Rect(0, 0, 600, 600).draw(ColorF(0.0, 1.0, 0.0, 0.88));
				font80(U"ゲーム終了！").draw(60, 150);
				font40(U"お疲れ様でした").draw(160, 250);

				Rect(100, 380, 400, 80).draw(ColorF(1.0, 1.0, 1.0, Click_D * 0.6 + 0.4));
				if (MouseX >= 100.0 && MouseX <= 500.0 && MouseY >= 380.0 && MouseY <= 460.0) Click_D = min(Click_D + Scene::DeltaTime() * 2.5, 1.0);
				else Click_D = max(Click_D - Scene::DeltaTime() * 2.5, 0.0);
				font40(U"リザルト画面").draw(170, 395, Color(0, 0, 0));
			}

			// マウスの位置判定
			int ty = 0, idx_a = -1, idx_b = -1;
			if (MouseX >= 90.0 && MouseX <= 510.0 && MouseY >= 90.0 && MouseY <= 510.0) {
				if ((int)(MouseY + 70.0) % 80 < 20 && MouseX >= 100.0 && MouseX <= 500.0) {
					ty = 1;
					idx_a = (int)(MouseY - 85.0) / 80; idx_a = min(5, max(0, idx_a));
					idx_b = (int)(MouseX - 100.0) / 80; idx_b = min(4, max(0, idx_b));
				}
				if ((int)(MouseX + 70.0) % 80 < 20 && MouseY >= 100.0 && MouseY <= 500.0) {
					ty = 2;
					idx_b = (int)(MouseX - 85.0) / 80; idx_b = min(5, max(0, idx_b));
					idx_a = (int)(MouseY - 100.0) / 80; idx_a = min(4, max(0, idx_a));
				}
			}
			for (int i = 0; i < 6; i++) {
				for (int j = 0; j < 6; j++) {
					if (j != 5) {
						if (ty == 1 && idx_a == i && idx_b == j) Click_B[i][j] = min(1.0, Click_B[i][j] + 4.0 * Scene::DeltaTime());
						else Click_B[i][j] = max(0.0, Click_B[i][j] - 4.0 * Scene::DeltaTime());
					}
					if (i != 5) {
						if (ty == 2 && idx_a == i && idx_b == j) Click_C[i][j] = min(1.0, Click_C[i][j] + 4.0 * Scene::DeltaTime());
						else Click_C[i][j] = max(0.0, Click_C[i][j] - 4.0 * Scene::DeltaTime());
					}
				}
			}

			// クリック時の判定
			if (Scene::Time() - GetLastClick >= 0.1 && MouseL.down() && Elapsed >= 0.0 && Elapsed < SEIGEN) {
				GetLastClick = Scene::Time();
				if (ty == 1) LineState_B[idx_a][idx_b] ^= true;
				if (ty == 2) LineState_C[idx_a][idx_b] ^= true;
			}
			if (Scene::Time() - GetLastClick >= 0.1 && MouseL.down() && Elapsed >= 0.0 && Elapsed > SEIGEN) {
				GetLastClick = Scene::Time();
				if (MouseX >= 100.0 && MouseX <= 500.0 && MouseY >= 380.0 && MouseY <= 460.0) {
					Situation = 6;
					YourScore.Score = CurrentScore;
					GetTime(YourScore);
					Rank[NumPlays] = YourScore; NumPlays += 1;
					sort(Rank, Rank + NumPlays);
				}
			}
		}

		// ----------------------------------------- Step #3. 遊び方 ---------------------------------------
		if (Situation == 3) {
			if (NumPage == 1) {
				Rect(120, 520, 250, 60).draw(ColorF(Click_A[4], Click_A[4], Click_A[4]));
				Rect(430, 520, 250, 60).draw(ColorF(Click_A[5], Click_A[5], Click_A[5]));
				font30(U"次ページへ").draw(170, 530, Color(0, 0, 0));
				font30(U"メニューに戻る").draw(450, 530, Color(0, 0, 0));

				font60(U"ルール説明 (1/2)").draw(150, 15);
				for (int i = 0; i <= 5; i++) Line(40, 130 + 60 * i, 340, 130 + 60 * i).draw(3, ColorF(0.2, 0.2, 0.28));
				for (int i = 0; i <= 5; i++) Line(40 + 60 * i, 130, 40 + 60 * i, 430).draw(3, ColorF(0.2, 0.2, 0.28));
				Circle(175, 145, 5).draw(Color(255, 255, 0));
				Circle(280, 335, 5).draw(Color(255, 255, 0));
				Circle(210, 360, 5).draw(Color(255, 255, 0));
				Circle(250, 240, 5).draw(Color(255, 255, 0));
				Circle(60, 385, 5).draw(Color(255, 255, 0));
				Circle(100, 245, 5).draw(Color(255, 0, 0));
				Circle(320, 185, 5).draw(Color(255, 0, 0));
				Circle(200, 270, 5).draw(Color(127, 127, 255));

				font15(U"①").draw(400, 120);
				font15(U"ボールが上下左右から降ってきます。あなたは").draw(430, 120);
				font15(U"5×5 のマス目の形を成す合計 60 本の棒を操作").draw(430, 145);
				font15(U"することができます。").draw(430, 170);

				font15(U"②").draw(400, 205);
				font15(U"棒をクリックすることによって、ボールを通す").draw(430, 205);
				font15(U"か通さないかの状態を反転することができます。").draw(430, 230);
				font15(U"ボールが「通さない状態の棒」に衝突した場合").draw(430, 255);
				font15(U"反射します。").draw(430, 280);

				font15(U"③").draw(400, 315);
				font15(U"ゲーム終了時に存在するボールに応じて点数が").draw(430, 315);
				font15(U"以下のように付きます：").draw(430, 340);
				font15(U"青色").draw(430, 365, Color(127, 127, 255)); font15(U"：5点／").draw(461, 365);
				font15(U"黄色").draw(520, 365, Color(255, 255, 0)); font15(U"：2点／").draw(551, 365);
				font15(U"赤色").draw(610, 365, Color(255, 0, 0)); font15(U"：-2点").draw(641, 365);

				font15(U"④").draw(400, 400);
				font15(U"制限時間は 150 秒であり、最後の 10 秒は新").draw(430, 400);
				font15(U"たなボールが出ません。できるだけ多くの点").draw(430, 425);
				font15(U"数を取りましょう！").draw(430, 450);
			}
			if (NumPage == 2) {
				Rect(120, 520, 250, 60).draw(ColorF(Click_A[4], Click_A[4], Click_A[4]));
				Rect(430, 520, 250, 60).draw(ColorF(Click_A[5], Click_A[5], Click_A[5]));
				font30(U"前ページへ").draw(170, 530, Color(0, 0, 0));
				font30(U"メニューに戻る").draw(450, 530, Color(0, 0, 0));

				font60(U"ルール説明 (2/2)").draw(150, 15);
				font20(U"得点").draw(115, 125); font20(U"ランク").draw(255, 125);
				font20(U"得点").draw(475, 125); font20(U"ランク").draw(615, 125);
				Line(70, 155, 370, 155).draw(2, Color(255, 255, 255));
				Line(430, 155, 730, 155).draw(2, Color(255, 255, 255));
				Line(200, 125, 200, 445).draw(2, Color(255, 255, 255));
				Line(560, 125, 560, 445).draw(2, Color(255, 255, 255));

				font18(Format(Ranker[21].second) + U"点以上").draw(85, 160);
				for (int i = 1; i < 11; i++) {
					if (Ranker[21 - i].second < 10) font18(Format(Ranker[21 - i].second) + U"～" + Format(Ranker[22 - i].second - 1) + U"点").draw(97, 160 + 25 * i);
					else if (Ranker[21 - i].second < 100) font18(Format(Ranker[21 - i].second) + U"～" + Format(Ranker[22 - i].second - 1) + U"点").draw(91, 160 + 25 * i);
					else font18(Format(Ranker[21 - i].second) + U"～" + Format(Ranker[22 - i].second - 1) + U"点").draw(79, 160 + 25 * i);
				}
				for (int i = 0; i < 11; i++) {
					if (Ranker[10 - i].second < 10) font18(Format(Ranker[10 - i].second) + U"～" + Format(Ranker[11 - i].second - 1) + U"点").draw(457, 160 + 25 * i);
					else if (Ranker[10 - i].second < 100) font18(Format(Ranker[10 - i].second) + U"～" + Format(Ranker[11 - i].second - 1) + U"点").draw(451, 160 + 25 * i);
					else font18(Format(Ranker[10 - i].second) + U"～" + Format(Ranker[11 - i].second - 1) + U"点").draw(439, 160 + 25 * i);
				}
				for (int i = 0; i < 11; i++) {
					int b1 = 255, b2 = 255, b3 = 255; if (i < 10) b3 = 0;
					if (Rank[0].Score < Ranker[21 - i].second && i < 10) {
						font18(U"???").draw(267, 160 + 25 * i, Color(b1, b2, b3));
					}
					else {
						font18(Ranker[21 - i].first).draw(285 - 57 * Ranker[21 - i].first.size() / 10, 160 + 25 * i, Color(b1, b2, b3));
					}
				}
				for (int i = 0; i < 11; i++) {
					font18(Ranker[10 - i].first).draw(645 - 57 * Ranker[10 - i].first.size() / 10, 160 + 25 * i);
				}
				font20(U"上表のように、得点に応じてランクが付けられます").draw(160, 465);
			}

			int idx = -1;
			if (MouseX >= 120.0 && MouseX <= 370.0 && MouseY >= 520.0 && MouseY <= 580.0) idx = 4;
			if (MouseX >= 430.0 && MouseX <= 680.0 && MouseY >= 520.0 && MouseY <= 580.0) idx = 5;
			if (idx != 4) Click_A[4] = max(0.3, Click_A[4] - 0.04); else Click_A[4] = min(0.9, Click_A[4] + 0.04);
			if (idx != 5) Click_A[5] = max(0.3, Click_A[5] - 0.04); else Click_A[5] = min(0.9, Click_A[5] + 0.04);
			if (Scene::Time() - GetLastClick >= 0.1 && MouseL.down()) {
				GetLastClick = Scene::Time();
				if (idx == 4) NumPage = 3 - NumPage;
				if (idx == 5) { Situation = 1; Click_A[4] = 0.3; Click_A[5] = 0.3; }
			}
		}

		// --------------------------------------- Step #4. ランキング -------------------------------------
		if (Situation == 4 || Situation == 5) {
			font80(U"ランキング").draw(200, 15);
			font20(U"順位").draw(130, 140, Color(255, 255, 0));
			font20(U"得点").draw(230, 140, Color(255, 255, 0));
			font20(U"レベル").draw(380, 140, Color(255, 255, 0));
			font20(U"プレイ時刻").draw(550, 140, Color(255, 255, 0));

			int YourRank = -1, ends = 447;
			for (int i = 0; i < NumPlays; i++) {
				if (Situation == 5 && YourScore == Rank[i]) YourRank = i;
			}
			if (Situation == 5 && YourRank >= 10) ends = 480;
			Line(90, 169, 710, 169).draw(3, Color(255, 255, 0));
			Line(200, 140, 200, ends).draw(2, Color(255, 255, 255));
			Line(320, 140, 320, ends).draw(2, Color(255, 255, 255));
			Line(500, 140, 500, ends).draw(2, Color(255, 255, 255));

			for (int i = 0; i < 11; i++) {
				if (i == 10 && (YourRank <= 9 || Situation == 4)) continue;
				int id = i; if (i == 10) id = YourRank;
				int idx[6] = { 170, 540, 576, 612, 648, 684 };
				int num[6] = { id + 1, Rank[id].v1, Rank[id].v2, Rank[id].v3, Rank[id].v4, Rank[id].v5 };
				int zahyou = 174 + i * 27; if (i == 10) zahyou = 453;
				double z1 = 1.0, z2 = 1.0, z3 = 1.0; if (id == YourRank) { z2 = 0.0; z3 = 0.0; }
				
				for (int j = 0; j < 6; j++) {
					if (num[j] < 10) font20(num[j]).draw(idx[j] - 12, zahyou, ColorF(z1, z2, z3));
					else if (num[j] < 100) font20(num[j]).draw(idx[j] - 24, zahyou, ColorF(z1, z2, z3));
					else if (num[j] < 1000) font20(num[j]).draw(idx[j] - 36, zahyou, ColorF(z1, z2, z3));
					else font20(num[j]).draw(idx[j] - 48, zahyou, ColorF(z1, z2, z3));
					if (j != 0 && num[j] < 10) font20(U"0").draw(idx[j] - 24, zahyou, ColorF(z1, z2, z3));
				}
				font20(U"/").draw(idx[1] + 1, zahyou, ColorF(z1, z2, z3));
				font20(U":").draw(idx[3] + 3, zahyou, ColorF(z1, z2, z3));
				font20(U":").draw(idx[4] + 3, zahyou, ColorF(z1, z2, z3));

				if (Rank[id].Score < 10) font20(Rank[id].Score).draw(258, zahyou, ColorF(z1, z2, z3));
				else if (Rank[id].Score < 100) font20(Rank[id].Score).draw(246, zahyou, ColorF(z1, z2, z3));
				else font20(Rank[id].Score).draw(234, zahyou, ColorF(z1, z2, z3));
				font20(GetRankToScore(Rank[id].Score)).draw(410 - 6 * GetRankToScore(Rank[id].Score).size(), zahyou, ColorF(z1, z2, z3));
			}

			font15(U"現在のプレイ者数：").draw(590, 490, Color(150, 150, 150));
			font15(NumPlays).draw(730, 490, Color(150, 150, 150));

			if (Situation == 4) font30(U"マウス左クリックで戻る").draw(200, 527, ColorF(Periodic::Sine0_1(1s)));
			if (Situation == 5) font30(U"マウス左クリックで終了").draw(200, 527, ColorF(Periodic::Sine0_1(1s)));

			if (Scene::Time() - GetLastClick >= 0.07 && MouseL.down() == true) {
				GetLastClick = Scene::Time();
				Situation = 1;
				init_all();
			}
		}

		// --------------------------------------- Step #6. 評価値推移 ----------------------------------
		if (Situation == 6) {
			font30(U"得点：").draw(20, 20);
			font30(CurrentScore).draw(130, 20);
			font30(U"評価：").draw(20, 75);
			if (YourScore.Score < 200) font30(GetRankToScore(YourScore.Score) + U"（次のランクまで " + Format(RankToNext(YourScore.Score)) + U" 点）").draw(130, 75);
			else font30(GetRankToScore(YourScore.Score)).draw(130, 75);

			Line(160, 217, 720, 217).draw(LineStyle::SquareDot, 2, Color(255, 255, 255));
			Line(160, 255, 720, 255).draw(LineStyle::SquareDot, 2, Color(255, 255, 255));
			Line(160, 292, 720, 292).draw(LineStyle::SquareDot, 2, Color(255, 255, 255));
			Line(160, 330, 720, 330).draw(LineStyle::SquareDot, 2, Color(255, 255, 255));
			Line(160, 367, 720, 367).draw(LineStyle::SquareDot, 2, Color(255, 255, 255));
			Line(160, 405, 720, 405).draw(LineStyle::SquareDot, 2, Color(255, 255, 255));
			Line(160, 442, 720, 442).draw(LineStyle::SquareDot, 2, Color(255, 255, 255));
			Line(160, 480, 160, 488).draw(2, Color(255, 255, 255));
			Line(216, 480, 216, 488).draw(2, Color(255, 255, 255));
			Line(272, 480, 272, 488).draw(2, Color(255, 255, 255));
			Line(328, 480, 328, 488).draw(2, Color(255, 255, 255));
			Line(384, 480, 384, 488).draw(2, Color(255, 255, 255));
			Line(440, 480, 440, 488).draw(2, Color(255, 255, 255));
			Line(496, 480, 496, 488).draw(2, Color(255, 255, 255));
			Line(552, 480, 552, 488).draw(2, Color(255, 255, 255));
			Line(608, 480, 608, 488).draw(2, Color(255, 255, 255));
			Line(664, 480, 664, 488).draw(2, Color(255, 255, 255));
			Line(720, 480, 720, 488).draw(2, Color(255, 255, 255));

			for (int i = 0; i < (int)(SEIGEN + 0.1); i += 2) {
				int v1 = 160.0 + 560.0 * i / SEIGEN;
				int v2 = 160.0 + 560.0 * (i + 2) / SEIGEN;
				double I = 480.0 - 300.0 * ScoreGraph[i] / 200.0;
				if (ScoreGraph[i] < 50) Rect(v1, I, v2 - v1, 480.0 - I).draw(ColorF(1.0, 0, 0, 0.6));
				else if (ScoreGraph[i] < 100) Rect(v1, I, v2 - v1, 480.0 - I).draw(ColorF(1.0, 1.0, 0, 0.6));
				else Rect(v1, I, v2 - v1, 480.0 - I).draw(ColorF(0.5, 0.5, 1.0, 0.6));
			}

			Line(160, 180, 160, 480).draw(4, Color(255, 255, 255));
			Line(720, 180, 720, 480).draw(4, Color(255, 255, 255));
			Line(160, 180, 720, 180).draw(4, Color(255, 255, 255));
			Line(160, 480, 720, 480).draw(4, Color(255, 255, 255));

			font20(U"200点").draw(90, 165);
			font20(U"175点").draw(90, 202);
			font20(U"150点").draw(90, 240);
			font20(U"125点").draw(90, 277);
			font20(U"100点").draw(90, 315);
			font20(U" 75点").draw(90, 352);
			font20(U" 50点").draw(90, 390);
			font20(U" 25点").draw(90, 427);
			for (int i = 1; i <= 9; i++) {
				int JIKAN = (int)(0.1 * i * SEIGEN);
				if (JIKAN < 10) font20(JIKAN).draw(154 + i * 56, 492);
				else if (JIKAN < 100) font20(JIKAN).draw(148 + i * 56, 492);
				else font20(JIKAN).draw(142 + i * 56, 492);
			}
			font20(U"[秒]").draw(700, 492);

			font30(U"マウス左クリックで順位表へ").draw(205, 530, ColorF(Periodic::Sine0_1(1.5s)));

			if (Scene::Time() - GetLastClick >= 0.1 && MouseL.down()) {
				GetLastClick = Scene::Time();
				Situation = 5;
			}
		}
	}

	output_Ranking();
	return;
}