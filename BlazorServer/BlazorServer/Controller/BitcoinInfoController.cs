using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Http;
using System.Threading.Tasks;
using BlazorServer.EF;
using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;


namespace BlazorServer.Controller
{
    [ApiController]
    [Route("api/[controller]")]
    public class BitcoinInfoController : ControllerBase
    {
        [HttpPost]
        public dynamic Post(dynamic request)
        {

            using (var db = new BlogContext())
            {
                var blogs = db.Blogs
                    .Where(b => b.Rating > 3)
                    .OrderBy(b => b.Url)
                    .ToList();
            }

            return new { Message = "message" };
        }
    }
}