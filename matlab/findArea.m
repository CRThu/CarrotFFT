function a = findArea(width,varargin)

   p = inputParser;
   addRequired(p,'width');
   addOptional(p,'height',1);
   addOptional(p,'tall',1);
   parse(p,width,varargin{:});
   
   params = p.Results;
   
   a = width * params.height * params.tall; 
end