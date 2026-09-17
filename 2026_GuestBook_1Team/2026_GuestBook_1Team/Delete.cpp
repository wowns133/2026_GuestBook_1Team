#include "Draw.h"
#include "Delete.h"

void AllDelete::all_clear(HWND hWnd, Draw& draw)
{
	draw.ac_lines();
	InvalidateRect(hWnd, NULL, TRUE);
}

void SingleDelete::single_clear(HWND hWnd, Draw& draw, LPARAM lParam)
{
	int erase_x = LOWORD(lParam); //lParam의 하위 16비트를 가져와 x좌표로 저장
	int erase_y = HIWORD(lParam); //lParam의 상위 16비트를 가져와 y좌표로 저장

	int erase_hit = 10; //지우개 인식 반경 설정

	const std::vector<DrawLineData>& lines = draw.getDrawnLines();
	/*
		const = 원본 데이터 값을 바꾸지 못하도록 선언
		& = 선 데이터들을 복제가 아닌 참조하도록 하여 메모리 효율성을 올리기 위함

	*/
	int target_index = -1;
	/*
		target_index = 지워버릴 대상 선의 인덱스 넘버 저장용 변수
		배열의 시작은 0부터 이기 때문에 아직 지울 대상이 없다는 의미로 유효하지 않은 상태의 초기 값을 넣어줌
	*/

	for (int i = (int)lines.size() - 1;i >= 0;i--)	///0부터 시작이라 -1 해줘야 배열크기에 맞춰서 시작함(배열은 0부터 시작임)
	{
		bool sc_hit = false;
		for (int j = 1; j < (int)lines[i].point_data.size();j++)	/// 0부터 시작하면 -1이 될 수도 있음(범위 오류)
		{
			int m_X1 = lines[i].point_data[j - 1].point.x; /// 이전 점의 X 좌표를 가져 옴
			int m_Y1 = lines[i].point_data[j - 1].point.y; /// 이전 점의 Y 좌표를 가져 옴

			int m_X2 = lines[i].point_data[j].point.x;   /// 현재 점의 X 좌표를 가져 옴
			int m_Y2 = lines[i].point_data[j].point.y;   /// 현재 점의 Y 좌표를 가져 옴

			int min_X = m_X1;
			int max_X = m_X2;

			if (m_X1 > m_X2)
			{
				min_X = m_X2;
				max_X = m_X1;
			}

			int min_Y = m_Y1;
			int max_Y = m_Y2;

			if (m_Y1 > m_Y2)
			{
				min_Y = m_Y2;
				max_Y = m_Y1;
			}

			if (erase_x >= min_X - 10 && erase_x <= max_X + 10 &&
				erase_y >= min_Y - 10 && erase_y <= max_Y + 10)
			{
				target_index = i; //지울 대상 선의 인덱스 번호를 target_index에 저장

				draw.sc_line(target_index); //target_index에 저장된 번호의 선을 drawn_lines_data에서 삭제

				draw.redrawAllLines(hWnd); //선이 삭제된 후 남아있는 선들을 다시 그림

				return; //한 개의 선을 삭제했으므로 다른 선을 추가로 삭제하지 않고 함수 종료
			}
		}
	}
}