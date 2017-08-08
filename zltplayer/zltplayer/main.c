//
//  main.c
//  zltplayer
//
//  Created by 权周 on 2017/8/8.
//  Copyright © 2017年 权周. All rights reserved.
//

#include <stdio.h>

#include <libavformat/avformat.h>

int main(const int args, const char *argv[]) {
    av_register_all();
    return 0;
}
