package startat.de.thegame.util.ecs;

import java.util.HashMap;
import java.util.Map;

public class ECSManager {
	public static Map<Integer, Entity> entities = new HashMap<Integer, Entity>();
	
	public static void addEntity(Entity entity){
		entities.put(entity.getId(), entity);
	}
	
	public static Entity getEntity(Integer id){
		return entities.get(id);
	}
	
}
