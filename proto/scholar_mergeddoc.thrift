// Copyright 2010-2012 Jike Inc. All Rights Reserved.
// Author: cuishiqi@jike.com

// change log:
// 2012-02-17 add download_url field
// 2012-02-14 add Reference struct, add PdfInfo struct
// 2012-02-14 add Conference field

namespace cpp scholar
namespace java scholar
namespace php scholar

struct Author {
  1: string name,          // 作者姓名
  2: string organization,  // 所属机构
  3: string email,         // 电子邮箱
  4: i32 paper_count,      // 发表论文数
  5: optional i64 id,  // 作者全局ID
}

struct DataSource {
  1: string name,     // 数据源名称
}

struct Publication {
  1: string name,       // 刊物名称
  2: i32 language,      // 刊物语言类型，-1:其他,0:中文,1:英文
}

struct Conference {
  1: string name,       // 会议名称
  2: i32 type,          // 会议类型，1:国内,2:国外
}

struct Reference {
  1: string title,
  2: list<string> authors,
  3: string pub_year,
  4: Publication publication
}

struct PdfInfo {
  1: optional string title,
  2: optional list<string> author,
  3: optional list<string> keywords,
  4: optional string summary,
  5: optional i32 mod_date,
  6: optional i32 create_date,
  7: optional string producer,
  8: optional string creator,
  9: optional i32 page_number
}

struct PaperMergedDoc {
  1: string title,           // 论文标题
  2: list<Author> authors,   // 论文作者列表
  3: string pub_year,        // 发表年份
  4: Publication publication,     // 出版刊物
  5: Conference conference,       // 发行会议
  6: DataSource data_source,      // 数据库来源
  7: string summary ,        // 论文摘要
  8: string content,         // 论文正文
  9: list<string> keywords,  // 论文关键词列表
  10: i32 paper_type,               // 论文类型， 0: 其他, 1: 学位, 2: 期刊, 3: 会议
  11: i32 paper_language,          // 语言类型,  -1: 其他, 0: 中文, 1:英文
  12: string paper_category,       // 论文主题类型
  13: i64 sign,              // 论文签名
  14: i32 quality_score,     // 论文质量得分，用于静态rank。
  15: optional i32 cited_count,          // 引用次数
  16: optional list<i64> citing_papers,  //引用它的论文doc id列表
  17: optional i32 copy_cnt,             // 同一篇论文出现在不同站点的次数
  18: optional list<Reference> references,        // Reference列表
  19: optional list<i64> reference_docs,          // Reference doc id列表
  20: optional list<i64> other_copys,             // 相同论文doc id列表
  21: optional string url,      // 页面url
  22: optional i64 docid,       // 页面id
  23: optional PdfInfo pdf_fields    // PDF格式的话有此结构
  24: optional string download_url,      // 下载页面url
}

