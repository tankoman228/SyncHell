# Навайбкоженный нейрослоп
# Позволяет по STF фичам разделить и классифицировать настроения по звучанию
# из моего датасета, где МУЗЫКА, КОНЕЧНО ЖЕ, СКАЧАНА 100% ЛЕГАЛЬНО

# запускать из терминала в директории самого датасета
# TODO: посмотреть, м.б. починить и использовать

import numpy as np
import os
from pathlib import Path
import pickle
from sklearn.neighbors import NearestNeighbors
from sklearn.cluster import KMeans
import matplotlib.pyplot as plt
import librosa
import soundfile as sf


def load_stf_features(stf_dir):
    """Загрузка всех STF-фич из бинарных файлов"""
    features = []
    file_paths = []
    
    stf_dir = Path(stf_dir)
    for stf_file in stf_dir.glob("*.stf"):
        try:
            # Читаем бинарный файл
            data = np.fromfile(stf_file, dtype=np.float32)
            
            # Разделяем на векторы по 256 элементов (каждый вектор - 1 секунда)
            n_vectors = len(data) // 256
            if n_vectors == 0:
                continue
                
            vectors = data[:n_vectors * 256].reshape(n_vectors, 256)
            
            # Добавляем все векторы из файла
            for i, vector in enumerate(vectors):
                features.append(vector)
                file_paths.append((stf_file.stem, i))  # (имя файла, секунда)
                
        except Exception as e:
            print(f"Ошибка при чтении {stf_file}: {e}")
    
    return np.array(features), file_paths

def find_centroids(features, n_clusters=67, method='nearest_to_center'):
    """Нахождение центроидов кластеров"""
    if method == 'nearest_to_center':
        # Используем K-means для нахождения кластеров
        kmeans = KMeans(n_clusters=n_clusters, random_state=42, n_init=10)
        labels = kmeans.fit_predict(features)
        
        centroids = []
        for cluster_id in range(n_clusters):
            # Векторы в текущем кластере
            cluster_vectors = features[labels == cluster_id]
            
            if len(cluster_vectors) == 0:
                # Если кластер пуст, используем центр K-means
                centroids.append(kmeans.cluster_centers_[cluster_id])
                continue
            
            # Центр кластера (математический)
            cluster_center = kmeans.cluster_centers_[cluster_id]
            
            # Находим вектор, ближайший к центру
            distances = np.linalg.norm(cluster_vectors - cluster_center, axis=1)
            nearest_idx = np.argmin(distances)
            centroid_vector = cluster_vectors[nearest_idx]
            
            centroids.append(centroid_vector)
        
        return np.array(centroids), labels
    
    elif method == 'kmeans':
        # Просто центроиды K-means
        kmeans = KMeans(n_clusters=n_clusters, random_state=42, n_init=10)
        kmeans.fit(features)
        return kmeans.cluster_centers_, kmeans.labels_

def save_centroids(centroids, output_file):
    """Сохранение центроидов в бинарный файл"""
    # Преобразуем в float32 и записываем последовательно
    centroids_flat = centroids.astype(np.float32).flatten()
    centroids_flat.tofile(output_file)
    
    # Также сохраняем в pickle для удобства дальнейшего использования
    pickle_file = output_file.replace('.bin', '.pkl')
    with open(pickle_file, 'wb') as f:
        pickle.dump(centroids, f)
    
    print(f"Центроиды сохранены в {output_file}")
    print(f"Размер центроидов: {centroids.shape} (кластеры × фичи)")

def visualize_clusters(features, labels, centroids, output_dir):
    """Визуализация кластеров с использованием PCA/TSNE"""
    from sklearn.decomposition import PCA
    from sklearn.manifold import TSNE
    
    output_dir = Path(output_dir)
    output_dir.mkdir(exist_ok=True)
    
    # PCA для 2D визуализации
    pca = PCA(n_components=2)
    features_2d = pca.fit_transform(features)
    centroids_2d = pca.transform(centroids)
    
    plt.figure(figsize=(12, 10))
    scatter = plt.scatter(features_2d[:, 0], features_2d[:, 1], 
                         c=labels, cmap='tab20', alpha=0.6, s=10)
    plt.scatter(centroids_2d[:, 0], centroids_2d[:, 1], 
               c='red', s=200, marker='X', edgecolors='black', linewidth=2)
    plt.colorbar(scatter)
    plt.title('K-Nearest Neighbors Clusters Visualization (PCA)')
    plt.xlabel('PCA Component 1')
    plt.ylabel('PCA Component 2')
    plt.tight_layout()
    plt.savefig(output_dir / 'clusters_pca.png', dpi=150)
    plt.close()
    
    # t-SNE для более качественной визуализации (на подмножестве данных)
    if len(features) > 10000:
        # Берем случайную подвыборку для t-SNE
        indices = np.random.choice(len(features), 10000, replace=False)
        features_sample = features[indices]
        labels_sample = labels[indices]
    else:
        features_sample = features
        labels_sample = labels
    
    tsne = TSNE(n_components=2, random_state=42, perplexity=30)
    features_tsne = tsne.fit_transform(features_sample)
    
    plt.figure(figsize=(12, 10))
    scatter = plt.scatter(features_tsne[:, 0], features_tsne[:, 1], 
                         c=labels_sample, cmap='tab20', alpha=0.6, s=10)
    plt.colorbar(scatter)
    plt.title('K-Nearest Neighbors Clusters Visualization (t-SNE)')
    plt.xlabel('t-SNE Component 1')
    plt.ylabel('t-SNE Component 2')
    plt.tight_layout()
    plt.savefig(output_dir / 'clusters_tsne.png', dpi=150)
    plt.close()

def create_audio_samples(centroids, features, file_paths, audio_dir, output_dir, 
                        sample_duration=2.0, sr=22050):
    """
    Создание аудио-образцов для каждого центроида
    Берет случайные фрагменты из оригинальных аудио-файлов
    """
    output_dir = Path(output_dir)
    output_dir.mkdir(exist_ok=True)
    
    audio_dir = Path(audio_dir)
    
    # Находим ближайший вектор к каждому центроиду
    nn = NearestNeighbors(n_neighbors=1, metric='euclidean')
    nn.fit(features)
    
    # Для каждого центроида находим ближайший вектор
    distances, indices = nn.kneighbors(centroids)
    
    # Словарь для группировки файлов по кластерам
    cluster_files = {}
    
    for cluster_id, (vector_idx, distance) in enumerate(zip(indices.flatten(), distances.flatten())):
        filename, second = file_paths[vector_idx]
        
        if cluster_id not in cluster_files:
            cluster_files[cluster_id] = []
        cluster_files[cluster_id].append((filename, second, distance))
    
    # Создаем аудио-образцы
    for cluster_id in range(len(centroids)):
        if cluster_id not in cluster_files:
            continue
            
        # Берем несколько примеров для каждого кластера
        examples = cluster_files[cluster_id]
        examples.sort(key=lambda x: x[2])  # Сортируем по расстоянию
        
        # Ограничиваем количество примеров
        examples = examples[:min(5, len(examples))]
        
        for i, (filename, second, distance) in enumerate(examples):
            # Ищем соответствующий аудио-файл
            audio_file = None
            for ext in ['.ogg', '.mp3', '.wav', '.flac']:
                potential_file = audio_dir / f"{filename}{ext}"
                if potential_file.exists():
                    audio_file = potential_file
                    break
            
            if audio_file is None:
                print(f"Не найден аудио-файл для {filename}")
                continue
            
            try:
                # Загружаем аудио
                audio, sample_rate = librosa.load(audio_file, sr=sr)
                
                # Вычисляем стартовую позицию (в сэмплах)
                start_sample = int(second * sr)
                end_sample = int((second + sample_duration) * sr)
                
                # Проверяем границы
                if start_sample < 0:
                    start_sample = 0
                if end_sample > len(audio):
                    end_sample = len(audio)
                    start_sample = max(0, end_sample - int(sample_duration * sr))
                
                # Извлекаем фрагмент
                audio_segment = audio[start_sample:end_sample]
                
                # Если фрагмент слишком короткий, добавляем тишину
                if len(audio_segment) < int(sample_duration * sr):
                    silence = np.zeros(int(sample_duration * sr) - len(audio_segment))
                    audio_segment = np.concatenate([audio_segment, silence])
                
                # Сохраняем
                output_file = output_dir / f"token_{cluster_id}_ex{i}.mp3"
                sf.write(output_file, audio_segment, sr)
                
                print(f"Создан образец: {output_file}")
                
            except Exception as e:
                print(f"Ошибка при создании образца для {filename}, секунда {second}: {e}")

def main():
    # Конфигурация
    STF_DIR = "STF"  # Папка с бинарниками STF
    AUDIO_DIR = "."   # Папка с оригинальными аудио-файлами
    OUTPUT_DIR = "output"
    N_CLUSTERS = 67
    
    # Создаем выходную директорию
    Path(OUTPUT_DIR).mkdir(exist_ok=True)
    
    print("Загрузка STF-фич...")
    features, file_paths = load_stf_features(STF_DIR)
    
    if len(features) == 0:
        print("Ошибка: Не найдены STF-фичи")
        return
    
    print(f"Загружено {len(features)} векторов из {len(set([fp[0] for fp in file_paths]))} файлов")
    
    print("Кластеризация и поиск центроидов...")
    centroids, labels = find_centroids(features, N_CLUSTERS, method='nearest_to_center')
    
    print("Сохранение центроидов...")
    save_centroids(centroids, Path(OUTPUT_DIR) / "centroids.bin")
    
    print("Визуализация кластеров...")
    visualize_clusters(features, labels, centroids, OUTPUT_DIR)
    
    print("Создание аудио-образцов...")
    create_audio_samples(centroids, features, file_paths, AUDIO_DIR, 
                        Path(OUTPUT_DIR) / "audio_samples", sample_duration=2.0)
    
    # Создаем файл с информацией о центроидах для C++ программы
    create_cpp_header(centroids, Path(OUTPUT_DIR) / "centroids.h")
    
    print("\nГотово!")
    print(f"Центроиды сохранены в: {OUTPUT_DIR}/centroids.bin")
    print(f"Аудио-образцы сохранены в: {OUTPUT_DIR}/audio_samples/")
    print(f"Визуализации сохранены в: {OUTPUT_DIR}/")

def create_cpp_header(centroids, output_file):
    """Создание заголовочного файла для C++ программы"""
    with open(output_file, 'w') as f:
        f.write("#ifndef CENTROIDS_H\n")
        f.write("#define CENTROIDS_H\n\n")
        f.write(f"#define NUM_CENTROIDS {centroids.shape[0]}\n")
        f.write(f"#define FEATURES_PER_VECTOR {centroids.shape[1]}\n\n")
        
        f.write("const float centroids[NUM_CENTROIDS][FEATURES_PER_VECTOR] = {\n")
        
        for i, centroid in enumerate(centroids):
            f.write(f"    {{  // Centroid {i}\n")
            f.write("        ")
            for j, value in enumerate(centroid):
                f.write(f"{value:.6f}f")
                if j < len(centroid) - 1:
                    f.write(", ")
                if (j + 1) % 8 == 0 and j < len(centroid) - 1:
                    f.write("\n        ")
            f.write("\n    }")
            if i < len(centroids) - 1:
                f.write(",")
            f.write("\n")
        
        f.write("};\n\n")
        f.write("#endif // CENTROIDS_H\n")

if __name__ == "__main__":
    main()