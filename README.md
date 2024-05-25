# SoftSDF
SDF密码设备接口的软件实现

## 目录
1. [项目简介](#项目简介)
2. [SDF 基本概念](#sdf-基本概念)
3. [SoftSDF 项目](#softsdf-项目)
4. [基本功能](#基本功能)
5. [softsdf 动态库](#softsdf-动态库)
6. [softsdfinit 命令行工具](#softsdfinit-命令行工具)
7. [softsdfinit 使用说明](#softsdfinit-使用说明)
8. [编译和安装](#编译和安装)
9. [示例](#示例)

## 项目简介

SoftSDF 是一个实现了 SDF 接口的软件项目。该项目编译后生成一个 SDF 动态库和一个名为 `softsdfinit` 的命令行工具。SoftSDF 提供标准的 SDF 接口，通过调用 GmSSL 库以软件方式实现密码计算和密钥存储。

## SDF 基本概念

SDF 是中国国家密码管理局发布的安全密码设备接口标准。该标准定义了密码设备与外部应用系统之间的接口，包括以下几个方面：
- **基础安全算法**：SM1、SM2、SM3、SM4 等中国国家密码算法。
- **密钥管理**：密钥的生成、存储、使用和销毁的管理规范。
- **数据加密解密**：数据加密和解密的接口和操作。
- **数字签名和验签**：数字签名的生成和验证接口。
- **消息摘要**：消息摘要算法（如 SM3）的计算接口。
- **身份认证**：身份认证的接口和操作流程。

## SoftSDF 项目

SoftSDF 项目包括 `softsdf` 动态库和 `softsdfinit` 命令行工具，用于实现和初始化一个虚拟的 SDF 设备。

## 基本功能

- **softsdf 动态库**：提供标准的 SDF 接口，通过 GmSSL 库以软件方式实现密码计算和密钥存储。
- **softsdfinit 命令行工具**：用于初始化一个虚拟的 SDF 设备，创建 KEK 密钥和 SM2 密钥对，并写入文件系统当前目录。

## softsdf 动态库

`softsdf` 动态库提供了标准的 SDF 接口，包括密钥管理、数据加密解密、数字签名和消息摘要等功能。该库使用 GmSSL 库以软件方式实现密码计算和密钥存储。

## softsdfinit 命令行工具

`softsdfinit` 是一个用于初始化虚拟 SDF 设备的命令行工具。该工具生成对称密钥 KEK 和 SM2 密钥对，并将它们存储在文件系统当前目录下。

## softsdfinit 使用说明

### 语法

```sh
softsdfinit -kek num -key num -pass str
```

### 参数说明

- `-kek num`：指定 KEK 的索引值，范围为 1 到 4096。
- `-key num`：指定私钥的索引值，范围为 0 到 4096。
- `-pass str`：用于访问私钥的密码字符串。

### 帮助信息

可以使用 `-help` 选项来查看工具的使用帮助：

```sh
$ softsdfinit -help
usage: softsdfinit -kek num -key num -pass str

Options

    -kek num            KEK index
    -key num            Private key index
    -pass str           Password for accessing the private key

Examples

  $ softsdfinit -kek 1 -key 1 -pass P@ssw0rd
```

### 示例

以下命令生成 KEK 索引为 1、私钥索引为 1，密码为 `P@ssw0rd` 的密钥和文件：

```sh
$ softsdfinit -kek 1 -key 1 -pass P@ssw0rd
```

### 文件存储

- **KEK 文件**：以 `kek-<index>.key` 命名，明文存储。
- **SM2 签名私钥文件**：以 `sm2sign-<index>.pem` 命名，采用 PKCS#8 口令加密 PEM 格式存储。
- **SM2 签名公钥文件**：以 `sm2signpub-<index>.pem` 命名，PEM 格式存储。
- **SM2 加密私钥文件**：以 `sm2enc-<index>.pem` 命名，采用 PKCS#8 口令加密 PEM 格式存储。
- **SM2 加密公钥文件**：以 `sm2encpub-<index>.pem` 命名，PEM 格式存储。

## 编译和安装

### 编译

首先以静态库的方式编译、安装GmSSL。从 https://github.com/guanzhi/GmSSL 下载GmSSL的源代码。在源代码目录执行如下操作：

```sh
mkdir build
cd build
cmake .. -DBUILD_SHARED_LIBS=OFF
make
make test
sudo make install
```

由于SoftSDF依赖GmSSL的静态库，因此应检查静态库`/usr/local/lib/libgmssl.a`是否存在。默认情况下编译GmSSL只生成动态库，通过指定`-DBUILD_SHARED_LIBS=OFF`可以生成静态库。

编译 SoftSDF 项目，生成 SDF 动态库和 `softsdfinit` 命令行工具：

```sh
mkdir build
cd build
cmake ..
make
```

### 安装

将生成的 `softsdfinit` 工具和 `softsdf` 动态库拷贝到合适的位置，例如 `/usr/local/bin` 和 `/usr/local/lib`。

## 示例

以下命令生成 KEK 索引为 1、私钥索引为 1，密码为 `P@ssw0rd` 的密钥和文件：

```sh
$ softsdfinit -kek 1 -key 1 -pass P@ssw0rd
```

生成的文件包括：
- `kek-1.key`
- `sm2sign-1.pem`
- `sm2signpub-1.pem`
- `sm2enc-1.pem`
- `sm2encpub-1.pem`

这些文件将存储在执行命令时的当前目录中。
