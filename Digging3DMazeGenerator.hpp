#ifndef INCLUDED_DIGGING_3D_MAZE_GENERATOR_HPP
#define INCLUDED_DIGGING_3D_MAZE_GENERATOR_HPP

#include <vector>
#include <tuple>


namespace Maze {

	template<class Matrix_Int_>
	class Digging3DMazeGenerator {
	private:

		const Matrix_Int_ empty_value;
		const Matrix_Int_ wall_value;

	public:

		//--------------------------------------------------------------------------------------------------
		// コンストラクタ
		Digging3DMazeGenerator(const Matrix_Int_& empty_value_, const Matrix_Int_& wall_value_)
			:empty_value(empty_value_), wall_value(wall_value_) {}

		//--------------------------------------------------------------------------------------------------
		// 3D迷路を生成
		template<typename Matrix_>
		void CreateMaze(Matrix_&& matrix_) {

			for (std::size_t z = 0; z < matrix_.size(); z++) {				
				for (std::size_t y = 0; y < matrix_[z].size(); y++) {
					for (std::size_t x = 0; x < matrix_[z][y].size(); x++) {
						matrix_[z][y][x] = wall_value;
					}
				}
			}
			

			if (matrix_.size() <= 2 || 
				matrix_[0].size() <= 2 || 
				matrix_[0][0].size() <= 2 ) {
				return;
			}


			bool finish_flag = false;
			std::size_t seed_x = 1;
			std::size_t seed_y = 1;
			std::size_t seed_z = 1;
			SetMazeChip(matrix_, seed_x, seed_y, seed_z, empty_value);

			while (!finish_flag) {
				const auto can_digging_dirs = GetCanDiggingDirs(matrix_, seed_x, seed_y, seed_z);
				if (can_digging_dirs.size() > 0) {
					const auto digging_dir = can_digging_dirs[rand() % can_digging_dirs.size()];
					const std::size_t dir_x =  std::get<0>(digging_dir);
					const std::size_t dir_y =  std::get<1>(digging_dir);
					const std::size_t dir_z =  std::get<2>(digging_dir);
					SetMazeChip(matrix_, seed_x + dir_x, seed_y + dir_y, seed_z + dir_z, empty_value);
					SetMazeChip(matrix_, seed_x + dir_x * 2, seed_y + dir_y * 2, seed_z + dir_z * 2, empty_value);
					seed_x += dir_x*2;
					seed_y += dir_y*2;
					seed_z += dir_z*2;
				}
				else {
					const auto can_respawn_seed_positions = GetCanRespawnSeedPositions(matrix_);
					if (can_respawn_seed_positions.size() > 0) {
						const auto respawn_pos = can_respawn_seed_positions[rand() % can_respawn_seed_positions.size()];
						seed_x = std::get<0>(respawn_pos);
						seed_y = std::get<1>(respawn_pos);
						seed_z = std::get<2>(respawn_pos);
						SetMazeChip(matrix_, seed_x, seed_y, seed_z, empty_value);
					}
					else {
						finish_flag = true;
					}
				}
			}

		}

	private:

		//--------------------------------------------------------------------------------------------------
		template<typename Matrix_>
		bool IsInRange(
			Matrix_&& matrix_,
			const std::size_t pos_x,
			const std::size_t pos_y,
			const std::size_t pos_z )
		{
			if (pos_z < 0 ||
				pos_y < 0 ||
				pos_x < 0 ||
				pos_z >= matrix_.size() ||
				pos_y >= matrix_[pos_z].size() ||
				pos_x >= matrix_[pos_z][pos_y].size()) {
				return false;
			}
			return true;
		}

		//--------------------------------------------------------------------------------------------------
		template<typename Matrix_>
		void SetMazeChip(
			Matrix_&& matrix_,
			const std::size_t pos_x,
			const std::size_t pos_y, 
			const std::size_t pos_z, 
			const Matrix_Int_ new_state) {

			if (IsInRange(matrix_, pos_x, pos_y, pos_z)) {
				matrix_[pos_z][pos_y][pos_x] = new_state;
			}
			
		}

		//--------------------------------------------------------------------------------------------------
		template<typename Matrix_>
		std::vector<std::tuple<std::size_t, std::size_t, std::size_t>> 
		GetCanDiggingDirs(
			Matrix_&& matrix_, 
			const std::size_t pos_x,
			const std::size_t pos_y, 
			const std::size_t pos_z ) 
		{
			std::vector<std::tuple<std::size_t, std::size_t, std::size_t>> result;
			static const std::tuple<int, int, int> dirs[] = { {-1,0,0},{+1,0,0},{0,-1,0},{0,+1,0},{0,0,-1},{0,0,+1} };
			for (const auto& dir : dirs) {
				const std::size_t check_x = (std::size_t)((int)pos_x + std::get<0>(dir)*2);
				const std::size_t check_y = (std::size_t)((int)pos_y + std::get<1>(dir)*2);
				const std::size_t check_z = (std::size_t)((int)pos_z + std::get<2>(dir)*2);
				if (IsInRange(matrix_, check_x, check_y, check_z) &&
					(static_cast<Matrix_Int_>(matrix_[check_z][check_y][check_x]) == wall_value)) {
					result.push_back({ std::get<0>(dir),std::get<1>(dir),std::get<2>(dir) });
				}
			}
			return result;
		}

		//--------------------------------------------------------------------------------------------------
		template<typename Matrix_>
		std::vector<std::tuple<std::size_t, std::size_t, std::size_t>>
			GetCanRespawnSeedPositions(Matrix_&& matrix_)
		{
			std::vector<std::tuple<std::size_t, std::size_t, std::size_t>> result;
			for (std::size_t z = 0; z < matrix_.size() / 2; z++) {
				for (std::size_t y = 0; y < matrix_[z*2].size() / 2; y++) {
					for (std::size_t x = 0; x < matrix_[z*2][y*2].size() / 2; x++) {
						const std::size_t check_x = x * 2 + 1;
						const std::size_t check_y = y * 2 + 1;
						const std::size_t check_z = z * 2 + 1;
						if (IsInRange(matrix_, check_x, check_y, check_z) &&
							(static_cast<Matrix_Int_>(matrix_[check_z][check_y][check_x]) == empty_value) &&
							GetCanDiggingDirs(matrix_, check_x, check_y, check_z).size() > 0) {
							result.push_back({ check_x, check_y, check_z });
						}
					}
				}
			}
			return result;
		}
	};

}


#endif // Included Gigging 3D Maze Generator