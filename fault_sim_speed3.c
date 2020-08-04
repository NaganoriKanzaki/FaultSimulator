#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//キュー
struct node {
  int data;
  struct node* next;
};

//関数のプロトタイプ宣言
void printList( struct node* head );  // キューの中身を表示
void  enqueue( struct node* sp, int x );  // エンキュー 
int is_empty( struct node* sp );      // キューが空ならば1をさもなくば0をreturnする 
int dequeue( struct node* sp );           // デキュー 
void push( struct node* sp, int x );     //プッシュ
void deter_calorder(int (*table_list1)[5], int* table_list2, struct node* sp, int line, int* sig_val); //出力の計算順序を決める
void deter_fault_calorder(int (*table_list1)[5], int* table_list2, struct node* sp, int line, int* sig_val); //故障時の出力の計算順序を決める
int logic_cal(int (*table_list1)[5], int* table_list2,  int line, int* sig_val); // 出力計算


int main(int argc, char *argv[]){
  int (*table_list1)[5];        // テーブルリスト1(信号線の構成情報)
  int *table_list2;        // テーブルリスト2(ポインタ情報)
  int *input_line;         // 入力線情報
  int *output_line;        // 出力線情報
  int **test_ptn;          // テストパターン情報
  int (*fault_list)[3];    // 故障リスト
  int *cal_order;          // 計算順序
  int *normal_sig_val;     // 正常時の信号線出力値
  int *fault_sig_val;      // 故障時の信号線出力値
  int **fault_cal_order;    // 故障時の計算順序
  int *fault_cal_order_length; //故障時の計算順序配列の要素数
  int sig_num;             // 信号線の数(リスト1の行数) 
  int list2_num;           // リスト2の行数 
  int inp_num;             // 外部入力線数
  int out_num;             // 外部出力線数
  int test_num;             // テストパターン数
  int fault_num;           // 故障パターン数
  int fault_detec_num = 0;      //故障検出数
  int i, j, k, l;
  FILE *fp;         // ファイル読み込み用
  struct node* sp;
  char filename[60];
  clock_t start, end;

  //回路テーブルの読み込み
  strcpy(filename, "./iscas85/");
  strcat(filename,argv[1]);
  strcat(filename,".tbl");

  if((fp = fopen(filename,"r")) == NULL){
    printf("tbl File open Error.\n");
    exit(-1);
  }

  //ここからリスト1の読み込み
  if(fscanf(fp,"%d",&sig_num) != 1){
    printf("リスト1行数読込エラー.\n");
    exit(-1);
  }
 
  table_list1 = malloc(sig_num * 5 * sizeof(int)); 

  for(i = 0; i < sig_num; i++){
    for( j = 0; j < 5; j++){
      if(fscanf(fp,"%d",&table_list1[i][j]) != 1){
	printf("リスト1情報読込エラー.\n");
	exit(-1);
      }
    }
  }


  //ここからリスト2の読み込み
  if(fscanf(fp,"%d",&list2_num) != 1){
    printf("リスト2行数読込エラー\n");
    exit(-1);
  }

  table_list2 = malloc(list2_num * sizeof(int)); 

  for(i = 0; i < list2_num; i++) { 
    if(fscanf(fp,"%d",&table_list2[i]) != 1){
      printf("リスト2情報読込エラー\n");
      exit(-1);
    }
  }

  //ここから入力線の読み込み
  if(fscanf(fp,"%d",&inp_num) != 1){
    printf("外部入力線数読込エラー\n");
    exit(-1);
  }

  input_line = malloc(inp_num * sizeof(int));

  for( i = 0; i < inp_num; i++){
    if(fscanf(fp,"%d",&input_line[i]) != 1){
      printf("外部入力線情報読込エラー\n");
      exit(-1);
    }
  }

  //ここから出力線の読み込み
  if(fscanf(fp,"%d",&out_num) != 1){
    printf("外部出力線数読込エラー\n");
    exit(-1);
  }
 
  output_line = malloc(out_num * sizeof(int));
  for( i = 0; i < out_num; i++){
    if(fscanf(fp,"%d",&output_line[i]) != 1){
      printf("外部入力線情報読込エラー\n");
      exit(-1);
    }
  }

  fclose(fp);          // ファイルをクローズ(閉じる)

  //テストパターン読み込み
  strcpy(filename, "./iscas85/");
  strcat(filename,argv[1]);
  strcat(filename,".pat");

  if((fp = fopen(filename,"r")) == NULL){
    printf("pat File open Error.\n");
    exit(-1);
  }

  if(fscanf(fp,"%d",&test_num) != 1){
    printf("テストパターン数読込エラー\n");
    exit(-1);
  }

  test_ptn = malloc(test_num * sizeof(int *));
  for (i = 0; i < test_num ; i++) {
    test_ptn[i] = malloc(inp_num * sizeof(int));
  }

 for( i = 0; i < test_num; i++){
    for( j = 0; j < inp_num; j++){
      if(fscanf(fp,"%d",&test_ptn[i][j]) != 1){
	printf("テストパターン情報読込エラー\n");
	exit(-1);
      }
    }
  }

  fclose(fp);          // ファイルをクローズ(閉じる)

 //故障リスト読み込み
  strcpy(filename, "./iscas85/");
  strcat(filename,argv[1]);
  strcat(filename,"f.rep");

  if((fp = fopen(filename,"r")) == NULL){
    printf("rep File open Error.\n");
    exit(-1);
  }

  if(fscanf(fp,"%d",&fault_num) != 1){
    printf("故障リスト行数読込エラー\n");
    exit(-1);
  }

  fault_list = malloc(fault_num * 3 * sizeof(int *));
  for(i = 0; i < fault_num; i++){
    for( j = 0; j < 2; j++){
      if(fscanf(fp,"%d",&fault_list[i][j]) != 1){
	printf("故障リスト情報読込エラー.\n");
	exit(-1);
      }
    }
  }

  fclose(fp);          // ファイルをクローズ(閉じる)

  //時間計測開始
  start = clock();

  /* 先頭にダミーノードを作っておく */
  sp = (struct node*) malloc(sizeof(struct node) );
  sp->next = NULL;

  //信号線出力値配列のメモリを確保
  normal_sig_val = malloc(sig_num * sizeof(int)); 
  fault_sig_val = malloc(sig_num * sizeof(int));

  //信号線出力値を未定義(-1)状態にする
  for( k = 0; k < sig_num; k++){
    normal_sig_val[k] = -1;
  }

  //外部入力線の出力値を計算順序決定済み状態(-2)にする
  for( j = 0 ; j < inp_num; j++){
    normal_sig_val[input_line[j]-1] = -2;
  }

  //計算順序配列のメモリを確保
  cal_order = (int *)malloc( ( sig_num - (inp_num + out_num) )* sizeof(int));
  //計算順序を外部出力線を一つずつ取り出しながら決めていく
  i = 0;
  for( l = 0; l < out_num; l++){
    deter_calorder(table_list1, table_list2, sp, output_line[l], normal_sig_val);
    //計算順序配列に格納
    while( !is_empty(sp) ){
      cal_order[i] = dequeue(sp);
      i++;
    }
  }

  /*確認用
  printf("計算順序\n");
  for( i = 0; i < ( sig_num - (inp_num + out_num) ); i++){
    printf("%d ", cal_order[i]);
  }
  printf("\n"); 
  */

  //故障時の計算順序配列のメモリを確保
  fault_cal_order = (int**)malloc( sig_num * sizeof(int *));
  for( i = 0; i < sig_num; i++){
    fault_cal_order[i] = (int*)malloc( sig_num * sizeof(int));
  }
  fault_cal_order_length = (int *)malloc( sig_num * sizeof(int));

  //ここからシミュレーション
  // テストパターンの割り当て
  for( i = 0; i < test_num; i++){
    //入力信号線にテストパターンを割り当てる
    for( j = 0 ; j < inp_num; j++){
      normal_sig_val[input_line[j]-1] = test_ptn[i][j];
    }
    //先ず正常時の出力値を計算
    for( k = 0; k < ( sig_num - (inp_num + out_num) ); k++){
      logic_cal(table_list1, table_list2, cal_order[k], normal_sig_val);
    }
    //次に故障時の出力値を計算
    for( l = 0; l < fault_num; l++){
      if( fault_list[l][2] != 1 && (normal_sig_val[ fault_list[l][0]-1 ] != fault_list[l][1]) ){
	if(fault_cal_order_length[fault_list[l][0]-1] == 0){
	  //ここから故障時の計算順序を求める
	  //故障時の信号線出力値を未定義(-1)状態にする
	  for( k = 0; k < sig_num; k++){
	    fault_sig_val[k] = -1;
	  }
	  deter_fault_calorder(table_list1, table_list2, sp, fault_list[l][0], fault_sig_val);
	  /*デバッグ用
	  printf("キュー\n");
	  printList(sp);
	  */
	  //故障時の計算順序配列に格納
	  fault_cal_order_length[fault_list[l][0]-1] = 0;
	  while( !is_empty(sp) ){
	    fault_cal_order[ fault_list[l][0]-1 ][ fault_cal_order_length[fault_list[l][0]-1] ] = dequeue(sp);
	    fault_cal_order_length[ fault_list[l][0]-1 ]++;
	  }
	}
	/*デバッグ用
	printf("テストパターンiの中身\n");
	printf("%d\n", i);
	printf("配列の要素数\n");
	printf("%d\n", fault_cal_order_length[fault_list[l][0]-1]);
	printf("故障している信号線\n");
	printf("%d\n", fault_list[l][0]-1);
	printf("配列の中身\n");
	for( k = 0; k < fault_cal_order_length[fault_list[l][0]-1]; k++){
	  printf("%d ", fault_cal_order[fault_list[l][0]-1][k] );
	}
	printf("\n");
	printf("-----------------------------------\n");
	*/
	//故障時の出力値に正常時の出力値をとりあえず代入する
	for( k = 0; k < sig_num; k++){
	  fault_sig_val[k] = normal_sig_val[k];
	}
	//故障割り当て
	fault_sig_val[ fault_list[l][0]-1 ] = fault_list[l][1];
	//ここから故障時の論理演算
	for( k = 1; k < fault_cal_order_length[fault_list[l][0]-1]; k++){
	  //もし0ならば直前で外部出力線を計算しているため
	  //故障時の計算結果と正しい計算結果を比較
	  if( fault_cal_order[fault_list[l][0]-1][k] == 0){
	    //故障を検出したならその故障を検出済み状態(1)にして、故障検出数をインクリメント
	    //さらにbreakで次の故障パターンへ移る
	    if( normal_sig_val[ fault_cal_order[ fault_list[l][0]-1 ][k-1] -1 ] != fault_sig_val[ fault_cal_order[ fault_list[l][0]-1 ][k-1] -1 ] ){
	      fault_list[l][2] = 1;
	      fault_detec_num++;
	      break;
	    }
	  }
	  else{
	    logic_cal(table_list1, table_list2, fault_cal_order[fault_list[l][0]-1][k], fault_sig_val);
	  }
	}
      }
    }
  }
  end = clock();

  printf("%6s ", argv[1]);
  //printf("パターン数 = %d ", test_num);
  printf("故障数=%4d ", fault_num);
  printf("故障検出数=%4d ", fault_detec_num);
  printf("故障検出率=%6.2f ", 100 * ((double)fault_detec_num)/fault_num );
  //printf("パターン数× 故障数 = %d\n ", test_num*fault_num);
  printf("処理時間= %.2f秒\n",(double)(end-start)/CLOCKS_PER_SEC);
  for(i = 0; i < 15; i++){
    printf("-----");
  }
  printf("\n");

  // 確保したメモリ領域の解放 
  free(table_list1);   
  free(table_list2);    
  free(input_line);  
  free(output_line);
  for (i = 0; i < test_num ; i++) {
    free(test_ptn[i]); 
  }
  free(cal_order); 
  free(fault_list);
  free(normal_sig_val);
  free(fault_sig_val);
  for( i = 0; i < sig_num; i++){
    free(fault_cal_order[i]);
  }
  free(fault_cal_order_length);

  return 0;
}


void printList( struct node* head ){
  struct node* p;
  p = head->next;
  while( p != NULL ){
    printf("%d ", p->data);
    p = p->next;
  }
  printf("\n");
}

void enqueue( struct node* head, int x ){
 struct node *p;
  p = head;
  while( p->next != NULL ){
    p = p->next;
  }
  p->next = (struct node*) malloc( sizeof(struct node) );
  p = p->next;
  p->data = x;
  p->next = NULL;
}

void push( struct node* sp, int x ){
  struct node *p;
  struct node *q;
  p = sp;
  q = p->next;
  p->next = (struct node*) malloc( sizeof(struct node) );
  p = p->next;
  p->data = x;
  p->next = q;
}

int is_empty( struct node* sp ){
  struct node *p;
  int x;
  x = 0;
  p = sp;

  if ( sp->next == NULL ){
    x = 1;
  }
  return x;
}

int dequeue( struct node* sp ){
  struct node* p;
  int x;
  if ( is_empty( sp ) ){
    return -1;
  }
  p = sp->next;
  x = p->data;

  struct node* q;
  p = sp;
  q = (p->next)->next;
  free(p->next);
  p->next = q;

  return x;
}

void deter_calorder(int (*table_list1)[5], int* table_list2, struct node* sp, int line, int* sig_val){
  int i;
  //信号線出力が未定義(-1)ならばキューに信号線番号を押し込み、その信号線の出力を計算順序決定済み(-2)にする
  if(sig_val[line-1] == -1){
    if(table_list1[line-1][1] == 1){
      deter_calorder(table_list1, table_list2, sp, table_list1[line-1][2], sig_val);
    }
    else{
      for( i = 0; i < table_list1[line-1][1]; i++){
	deter_calorder(table_list1, table_list2, sp, table_list2[ table_list1[line-1][2]-1+i ], sig_val);
      }
    }
    enqueue(sp, line);
    sig_val[line-1] = -2;
  }
}

void deter_fault_calorder(int (*table_list1)[5], int* table_list2, struct node* sp, int line, int* sig_val){
  int i;
  //信号線出力が未定義(-1)かつ出力先が存在するならばスタックに信号線番号をpushし、その信号線の出力を計算順序決定済み(-2)にする
  if( sig_val[line-1] == -1 && table_list1[line-1][3] != 0 ){
    if(table_list1[line-1][3] == 1){
      deter_fault_calorder(table_list1, table_list2, sp, table_list1[line-1][4], sig_val);
    }
    else{
      for( i = 0; i < table_list1[line-1][3]; i++){
	deter_fault_calorder(table_list1, table_list2, sp, table_list2[ table_list1[line-1][4]-1+i ], sig_val);
      }
    }
    push(sp, line);
    sig_val[line-1] = -2;
  }
  //出力先が存在しないならば、直前は外部出力線の計算をしているのでそれがわかるように0をpushする
  else if(table_list1[line-1][3] == 0){
    push(sp, 0);
  }
}

int logic_cal(int (*table_list1)[5], int* table_list2,  int line, int* sig_val){
  int i;
  int result = 0;

  switch(table_list1[line-1][0]){
  case 1: // OR
    if(table_list1[line-1][1] == 1){
      result = sig_val[ table_list1[line-1][2] -1 ];
    }
    else{
      for( i = 0; i < table_list1[line-1][1]; i++){
	result +=sig_val[ table_list2[ table_list1[line-1][2]-1+i ]-1 ];
      }
      if(result != 0){
	result = 1;
      }
    }
    sig_val[line-1] = result;
    break;
  case 2: // AND
    if(table_list1[line-1][1] == 1){
      result = sig_val[ table_list1[line-1][2] -1 ];
    }
    else{
      result = 1;
      for( i = 0; i < table_list1[line-1][1]; i++){
	if(sig_val[ table_list2[ table_list1[line-1][2]-1+i ]-1 ] < 0){
	  printf("エラー：出力が決まっていない信号線の値を用いています\n");
	  exit(-1);
	  }
	result *= sig_val[ table_list2[ table_list1[line-1][2]-1+i ]-1 ];
      }
    }
    sig_val[line-1] = result;
    break;
  case 3: //分岐の枝
    sig_val[line-1] = sig_val[ table_list1[line-1][2] -1 ];
    break;
  case 5: //EXOR
    if(table_list1[line-1][1] == 1){
      result = sig_val[ table_list1[line-1][2] -1 ];
    }
    else{
      for( i = 0; i < table_list1[line-1][1]; i++){
	result += sig_val[ table_list2[ table_list1[line-1][2]-1+i ]-1 ];
      }
      if( (result%2) == 0){
	result = 0;
      }
      else{
	result = 1;
      }
    }
    sig_val[line-1] = result;
    break;
  case -1: //NAND
    if(table_list1[line-1][1] == 1){
      result = sig_val[ table_list1[line-1][2] -1 ];
      if(result == 0){
	result = 1;
      }
      else{
	result = 0;
      }
    }
    else{
      result = 1;
      for( i = 0; i < table_list1[line-1][1]; i++){
	result *= sig_val[ table_list2[ table_list1[line-1][2]-1+i ]-1 ];
      }
      if(result == 0){
	result = 1;
      }
      else{
	result = 0;
      }
    }
    sig_val[line-1] = result;
    break;
  case -2: // NOR
    if(table_list1[line-1][1] == 1){
      result = sig_val[ table_list1[line-1][2] -1 ];
      if(result == 0){
	result = 1;
      }
      else{
	result = 0;
      }
    }
    else{
      for( i = 0; i < table_list1[line-1][1]; i++){
	result += sig_val[ table_list2[ table_list1[line-1][2]-1+i ]-1 ];
      }
      if(result != 0){
	result = 0;
      }
      else{
	result = 1;
      }
    }
    sig_val[line-1] = result;
    break;
  case -3: //NOT
    result = sig_val[ table_list1[line-1][2] -1 ];
    if(result == 0){
      result = 1;
    }
    else{
      result = 0;
    }
    sig_val[line-1] = result;
    break;
  default:
    printf("出力計算エラー\n");
    exit(-1);
    break;
  }
}
