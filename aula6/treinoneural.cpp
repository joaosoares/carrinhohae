//adapt01.cpp: Igual ao cnn_adam25.cpp 
//  Com inicial optimizer.alpha *= 0.03;
//  Multiplica alpha por 0.5 quando para de melhorar
// $compila adapt01 -c -t
#include <cektiny.h>
string data_dir_path="/home/parallels/cekeikon5/tiny_dnn/data/";

int main(int argc, char **argv) {
  int n_train_epochs=30;
  int n_minibatch=10;
  
  network<sequential> net;
  net << conv(28, 28, 5, 1, 20) 
      << max_pool(24, 24, 20, 2) << relu() 
      << conv(12, 12, 5, 20, 40) 
      << max_pool(8, 8, 40, 2) << relu()
      << fc(4*4*40, 1000) << relu()
      << fc(1000, 100) << relu()
      << fc(100, 10);

  vector<label_t> train_labels;
  vector<vec_t> train_images;
  parse_mnist_labels(data_dir_path + "train-labels.idx1-ubyte", &train_labels);
  parse_mnist_images(data_dir_path + "train-images.idx3-ubyte", &train_images, -1.0, 1.0, 0, 0);
  aumentaTreino(train_images,train_labels,28,28,1,false);

  vector<label_t> test_labels;
  vector<vec_t> test_images;
  parse_mnist_labels(data_dir_path + "t10k-labels.idx1-ubyte", &test_labels);
  parse_mnist_images(data_dir_path + "t10k-images.idx3-ubyte", &test_images, -1.0, 1.0, 0, 0);
    
  adam optimizer;
  optimizer.alpha *= 0.03;
  cout << "Learning rate=" << optimizer.alpha << endl;
  int sucesso_anterior=0;

  cout << "start training" << endl;
  progress_display disp(train_images.size());
  timer t;

  int epoch = 1;
  int omelhor=-1;
  auto on_enumerate_epoch = [&]() {
    cout << "Epoch " << epoch << "/" << n_train_epochs << " finished. "
              << t.elapsed() << "s elapsed. ";
    ++epoch;
    result res = net.test(test_images, test_labels);
    cout << res.num_success << "/" << res.num_total << endl;
    if (res.num_success<=sucesso_anterior) {
      optimizer.alpha *= 0.5;
      cout << "Learning rate=" << optimizer.alpha << endl;
    }
    sucesso_anterior=res.num_success;

    if (omelhor<=res.num_success) {
      omelhor=res.num_success;
      string nomearq = semDiret(string(argv[0]))+".net";
      net.save(nomearq);
      cout << "Gravado arquivo " << nomearq << endl;
    }

    disp.restart(train_images.size());
    t.restart();
  };

  auto on_enumerate_minibatch = [&]() { disp += n_minibatch; };

  net.train<mse>(optimizer, train_images, train_labels, n_minibatch,
                 n_train_epochs, on_enumerate_minibatch, on_enumerate_epoch);

  cout << "end training." << endl;
}

