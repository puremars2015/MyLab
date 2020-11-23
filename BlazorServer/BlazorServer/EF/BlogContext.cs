using Microsoft.EntityFrameworkCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace BlazorServer.EF
{
    public class BlogContext : DbContext
    {
        public DbSet<Blog> Blogs { get; set; }
        public DbSet<Post> Posts { get; set; }

        protected override void OnConfiguring(DbContextOptionsBuilder options)
            => options.UseSqlServer("Data Source=192.168.1.120;Persist Security Info=False;User ID=sa;Password=7527540a;Initial Catalog=MyLab;");

        public class Blog
        {
            public int BlogId { get; set; }
            public string Url { get; set; }
            public int Rating { get; set; }
            public List<Post> Posts { get; } = new List<Post>();
        }

        public class Post
        {
            public int PostId { get; set; }
            public string Title { get; set; }
            public string Content { get; set; }

            public int BlogId { get; set; }
            public Blog Blog { get; set; }
        }
    }
}
